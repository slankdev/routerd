/* Hash routine.
 * Copyright (C) 1998 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; see the file COPYING; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <zebra.h>
#include <math.h>

#include "hash.h"
#include "memory.h"
#include "linklist.h"
/* #include "termtable.h" */
#include "vty.h"
#include "command.h"

DEFINE_MTYPE(LIB, HASH, "Hash")
DEFINE_MTYPE(LIB, HASH_BACKET, "Hash Bucket")
DEFINE_MTYPE_STATIC(LIB, HASH_INDEX, "Hash Index")

static pthread_mutex_t _hashes_mtx = PTHREAD_MUTEX_INITIALIZER;
static struct list *_hashes;

struct hash *hash_create_size(unsigned int size,
			      unsigned int (*hash_key)(const void *),
			      bool (*hash_cmp)(const void *, const void *),
			      const char *name)
{
	struct hash *hash;

	assert((size & (size - 1)) == 0);
	hash = XCALLOC(MTYPE_HASH, sizeof(struct hash));
	hash->index =
		XCALLOC(MTYPE_HASH_INDEX, sizeof(struct hash_bucket *) * size);
	hash->size = size;
	hash->hash_key = hash_key;
	hash->hash_cmp = hash_cmp;
	hash->count = 0;
	hash->name = name ? XSTRDUP(MTYPE_HASH, name) : NULL;
	hash->stats.empty = hash->size;

	pthread_mutex_lock(&_hashes_mtx);
	{
		if (!_hashes)
			_hashes = list_new();

		listnode_add(_hashes, hash);
	}
	pthread_mutex_unlock(&_hashes_mtx);

	return hash;
}

struct hash *hash_create(unsigned int (*hash_key)(const void *),
			 bool (*hash_cmp)(const void *, const void *),
			 const char *name)
{
	return hash_create_size(HASH_INITIAL_SIZE, hash_key, hash_cmp, name);
}

void *hash_alloc_intern(void *arg)
{
	return arg;
}

#define hash_update_ssq(hz, old, new)                                          \
	atomic_fetch_add_explicit(&hz->stats.ssq, (new + old) * (new - old),   \
				  memory_order_relaxed);

/* Expand hash if the chain length exceeds the threshold. */
static void hash_expand(struct hash *hash)
{
	unsigned int i, new_size;
	struct hash_bucket *hb, *hbnext, **new_index;

	new_size = hash->size * 2;

	if (hash->max_size && new_size > hash->max_size)
		return;

	new_index = XCALLOC(MTYPE_HASH_INDEX,
			    sizeof(struct hash_bucket *) * new_size);

	hash->stats.empty = new_size;

	for (i = 0; i < hash->size; i++)
		for (hb = hash->index[i]; hb; hb = hbnext) {
			unsigned int h = hb->key & (new_size - 1);

			hbnext = hb->next;
			hb->next = new_index[h];

			int oldlen = hb->next ? hb->next->len : 0;
			int newlen = oldlen + 1;

			if (newlen == 1)
				hash->stats.empty--;
			else
				hb->next->len = 0;

			hb->len = newlen;

			hash_update_ssq(hash, oldlen, newlen);

			new_index[h] = hb;
		}

	/* Switch to new table */
	XFREE(MTYPE_HASH_INDEX, hash->index);
	hash->size = new_size;
	hash->index = new_index;
}

void *hash_get(struct hash *hash, void *data, void *(*alloc_func)(void *))
{
	unsigned int key;
	unsigned int index;
	void *newdata;
	struct hash_bucket *bucket;

	if (!alloc_func && !hash->count)
		return NULL;

	key = (*hash->hash_key)(data);
	index = key & (hash->size - 1);

	for (bucket = hash->index[index]; bucket != NULL;
	     bucket = bucket->next) {
		if (bucket->key == key && (*hash->hash_cmp)(bucket->data, data))
			return bucket->data;
	}

	if (alloc_func) {
		newdata = (*alloc_func)(data);
		if (newdata == NULL)
			return NULL;

		if (HASH_THRESHOLD(hash->count + 1, hash->size)) {
			hash_expand(hash);
			index = key & (hash->size - 1);
		}

		bucket = XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
		bucket->data = newdata;
		bucket->key = key;
		bucket->next = hash->index[index];
		hash->index[index] = bucket;
		hash->count++;

		int oldlen = bucket->next ? bucket->next->len : 0;
		int newlen = oldlen + 1;

		if (newlen == 1)
			hash->stats.empty--;
		else
			bucket->next->len = 0;

		bucket->len = newlen;

		hash_update_ssq(hash, oldlen, newlen);

		return bucket->data;
	}
	return NULL;
}

void *hash_lookup(struct hash *hash, void *data)
{
	return hash_get(hash, data, NULL);
}

unsigned int string_hash_make(const char *str)
{
	unsigned int hash = 0;

	while (*str)
		hash = (hash * 33) ^ (unsigned int)*str++;

	return hash;
}

void *hash_release(struct hash *hash, void *data)
{
	void *ret;
	unsigned int key;
	unsigned int index;
	struct hash_bucket *bucket;
	struct hash_bucket *pp;

	key = (*hash->hash_key)(data);
	index = key & (hash->size - 1);

	for (bucket = pp = hash->index[index]; bucket; bucket = bucket->next) {
		if (bucket->key == key
		    && (*hash->hash_cmp)(bucket->data, data)) {
			int oldlen = hash->index[index]->len;
			int newlen = oldlen - 1;

			if (bucket == pp)
				hash->index[index] = bucket->next;
			else
				pp->next = bucket->next;

			if (hash->index[index])
				hash->index[index]->len = newlen;
			else
				hash->stats.empty++;

			hash_update_ssq(hash, oldlen, newlen);

			ret = bucket->data;
			XFREE(MTYPE_HASH_BACKET, bucket);
			hash->count--;
			return ret;
		}
		pp = bucket;
	}
	return NULL;
}

void hash_iterate(struct hash *hash, void (*func)(struct hash_bucket *, void *),
		  void *arg)
{
	unsigned int i;
	struct hash_bucket *hb;
	struct hash_bucket *hbnext;

	for (i = 0; i < hash->size; i++)
		for (hb = hash->index[i]; hb; hb = hbnext) {
			/* get pointer to next hash bucket here, in case (*func)
			 * decides to delete hb by calling hash_release
			 */
			hbnext = hb->next;
			(*func)(hb, arg);
		}
}

void hash_walk(struct hash *hash, int (*func)(struct hash_bucket *, void *),
	       void *arg)
{
	unsigned int i;
	struct hash_bucket *hb;
	struct hash_bucket *hbnext;
	int ret = HASHWALK_CONTINUE;

	for (i = 0; i < hash->size; i++) {
		for (hb = hash->index[i]; hb; hb = hbnext) {
			/* get pointer to next hash bucket here, in case (*func)
			 * decides to delete hb by calling hash_release
			 */
			hbnext = hb->next;
			ret = (*func)(hb, arg);
			if (ret == HASHWALK_ABORT)
				return;
		}
	}
}

void hash_clean(struct hash *hash, void (*free_func)(void *))
{
	unsigned int i;
	struct hash_bucket *hb;
	struct hash_bucket *next;

	for (i = 0; i < hash->size; i++) {
		for (hb = hash->index[i]; hb; hb = next) {
			next = hb->next;

			if (free_func)
				(*free_func)(hb->data);

			XFREE(MTYPE_HASH_BACKET, hb);
			hash->count--;
		}
		hash->index[i] = NULL;
	}

	hash->stats.ssq = 0;
	hash->stats.empty = hash->size;
}

static void hash_to_list_iter(struct hash_bucket *hb, void *arg)
{
	struct list *list = arg;

	listnode_add(list, hb->data);
}

struct list *hash_to_list(struct hash *hash)
{
	struct list *list = list_new();

	hash_iterate(hash, hash_to_list_iter, list);
	return list;
}

void hash_free(struct hash *hash)
{
	pthread_mutex_lock(&_hashes_mtx);
	{
		if (_hashes) {
			listnode_delete(_hashes, hash);
			if (_hashes->count == 0) {
				list_delete(&_hashes);
			}
		}
	}
	pthread_mutex_unlock(&_hashes_mtx);

	XFREE(MTYPE_HASH, hash->name);

	XFREE(MTYPE_HASH_INDEX, hash->index);
	XFREE(MTYPE_HASH, hash);
}

/* The golden ration: an arbitrary value */
#define JHASH_GOLDEN_RATIO  0x9e3779b9

/* NOTE: Arguments are modified. */
#define __jhash_mix(a, b, c)                                                   \
	{                                                                      \
		a -= b;                                                        \
		a -= c;                                                        \
		a ^= (c >> 13);                                                \
		b -= c;                                                        \
		b -= a;                                                        \
		b ^= (a << 8);                                                 \
		c -= a;                                                        \
		c -= b;                                                        \
		c ^= (b >> 13);                                                \
		a -= b;                                                        \
		a -= c;                                                        \
		a ^= (c >> 12);                                                \
		b -= c;                                                        \
		b -= a;                                                        \
		b ^= (a << 16);                                                \
		c -= a;                                                        \
		c -= b;                                                        \
		c ^= (b >> 5);                                                 \
		a -= b;                                                        \
		a -= c;                                                        \
		a ^= (c >> 3);                                                 \
		b -= c;                                                        \
		b -= a;                                                        \
		b ^= (a << 10);                                                \
		c -= a;                                                        \
		c -= b;                                                        \
		c ^= (b >> 15);                                                \
	}

/* The most generic version, hashes an arbitrary sequence
 * of bytes.  No alignment or length assumptions are made about
 * the input key.
 */
uint32_t jhash(const void *key, uint32_t length, uint32_t initval)
{
	uint32_t a, b, c, len;
	const uint8_t *k = key;

	len = length;
	a = b = JHASH_GOLDEN_RATIO;
	c = initval;

	while (len >= 12) {
		a += (k[0] + ((uint32_t)k[1] << 8) + ((uint32_t)k[2] << 16)
		      + ((uint32_t)k[3] << 24));
		b += (k[4] + ((uint32_t)k[5] << 8) + ((uint32_t)k[6] << 16)
		      + ((uint32_t)k[7] << 24));
		c += (k[8] + ((uint32_t)k[9] << 8) + ((uint32_t)k[10] << 16)
		      + ((uint32_t)k[11] << 24));

		__jhash_mix(a, b, c);

		k += 12;
		len -= 12;
	}

	c += length;
	switch (len) {
	case 11:
		c += ((uint32_t)k[10] << 24);
	/* fallthru */
	case 10:
		c += ((uint32_t)k[9] << 16);
	/* fallthru */
	case 9:
		c += ((uint32_t)k[8] << 8);
	/* fallthru */
	case 8:
		b += ((uint32_t)k[7] << 24);
	/* fallthru */
	case 7:
		b += ((uint32_t)k[6] << 16);
	/* fallthru */
	case 6:
		b += ((uint32_t)k[5] << 8);
	/* fallthru */
	case 5:
		b += k[4];
	/* fallthru */
	case 4:
		a += ((uint32_t)k[3] << 24);
	/* fallthru */
	case 3:
		a += ((uint32_t)k[2] << 16);
	/* fallthru */
	case 2:
		a += ((uint32_t)k[1] << 8);
	/* fallthru */
	case 1:
		a += k[0];
	}

	__jhash_mix(a, b, c);

	return c;
}

/* A special optimized version that handles 1 or more of uint32_ts.
 * The length parameter here is the number of uint32_ts in the key.
 */
uint32_t jhash2(const uint32_t *k, uint32_t length, uint32_t initval)
{
	uint32_t a, b, c, len;

	a = b = JHASH_GOLDEN_RATIO;
	c = initval;
	len = length;

	while (len >= 3) {
		a += k[0];
		b += k[1];
		c += k[2];
		__jhash_mix(a, b, c);
		k += 3;
		len -= 3;
	}

	c += length * 4;

	switch (len) {
	case 2:
		b += k[1];
	/* fallthru */
	case 1:
		a += k[0];
	}

	__jhash_mix(a, b, c);

	return c;
}


/* A special ultra-optimized versions that knows they are hashing exactly
 * 3, 2 or 1 word(s).
 *
 * NOTE: In partilar the "c += length; __jhash_mix(a,b,c);" normally
 *       done at the end is not done here.
 */
uint32_t jhash_3words(uint32_t a, uint32_t b, uint32_t c, uint32_t initval)
{
	a += JHASH_GOLDEN_RATIO;
	b += JHASH_GOLDEN_RATIO;
	c += initval;

	__jhash_mix(a, b, c);

	return c;
}

uint32_t jhash_2words(uint32_t a, uint32_t b, uint32_t initval)
{
	return jhash_3words(a, b, 0, initval);
}

uint32_t jhash_1word(uint32_t a, uint32_t initval)
{
	return jhash_3words(a, 0, 0, initval);
}
