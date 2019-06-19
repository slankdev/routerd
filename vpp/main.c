
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <vnet/session/application_interface.h>
#include <vlibmemory/api.h>
#include <vpp/api/vpe_msg_enum.h>
#include <svm/fifo_segment.h>
#define vl_typedefs
#include <vpp/api/vpe_all_api_h.h>
#undef vl_typedefs
#define vl_endianfun
#include <vpp/api/vpe_all_api_h.h>
#undef vl_endianfun
#define vl_print(handle, ...)
#define vl_printfun
#include <vpp/api/vpe_all_api_h.h>
#undef vl_printfun
#define CONTROL_PING_MESSAGE "control_ping"
#define CONTROL_PING_REPLY_MESSAGE "control_ping_reply"

typedef struct
{
  unix_shared_memory_queue_t * vl_input_queue;
  u32 my_client_index;
} routerd_main_t;
routerd_main_t routerd_main;

inline static uint32_t
find_msg_id(char* msg) {
  api_main_t * am = &api_main;
  hash_pair_t *hp;
  hash_foreach_pair (hp, am->msg_index_by_name_and_crc, ({
       char *key = (char *)hp->key; // key format: name_crc
       int msg_name_len = strlen(key) - 9; // ignore crc
       if (strlen(msg) == msg_name_len &&
           strncmp(msg, (char *)hp->key, msg_name_len) == 0) {
         return (u32)hp->value[0];
       }
   }));
}

static void
stop_signal (int signum)
{
  routerd_main_t *rm = &routerd_main;
}

static void
stats_signal (int signum)
{
  routerd_main_t *rm = &routerd_main;
}

static clib_error_t *
setup_signal_handlers (void)
{
  signal (SIGINT, stats_signal);
  signal (SIGQUIT, stop_signal);
  signal (SIGTERM, stop_signal);
  return 0;
}

static int
send_ping(u16 ping_id, u16 msg_id) {
  routerd_main_t *rm = &routerd_main;

  vl_api_control_ping_t * mp =
    vl_msg_api_alloc(sizeof(vl_api_control_ping_t));
  memset(mp, 0, sizeof(*mp));
  mp->_vl_msg_id = ntohs(ping_id);
  mp->client_index = rm->my_client_index;
  mp->context = htonl(msg_id);
  vl_msg_api_send_shmem(rm->vl_input_queue, (u8 *) &mp);
  printf("Sending ping. Ping msg id: %u, message id: %u\n", ping_id, msg_id);
}

static void
vl_api_control_ping_reply_t_handler(vl_api_control_ping_reply_t * mp) {
    printf("\tPing reply received, message id: %d, with retval: %d\n",
        htonl(mp->context), mp->retval);
}

int
connect_to_vpp (char *name)
{
  routerd_main_t *rm = &routerd_main;
  api_main_t *am = &api_main;
  if (vl_client_connect_to_vlib("/vpe-api", name, 32) < 0) {
    clib_warning ("shmem connect failed");
    return -1;
  }

  rm->my_client_index = am->my_client_index;
  rm->vl_input_queue = am->shmem_hdr->vl_input_queue;

  vl_msg_api_set_handlers(
			find_msg_id(CONTROL_PING_REPLY_MESSAGE),
			CONTROL_PING_REPLY_MESSAGE,
      vl_api_control_ping_reply_t_handler, vl_noop_handler,
      vl_api_control_ping_reply_t_endian,
      vl_api_control_ping_reply_t_print,
      sizeof(vl_api_control_ping_reply_t), 1);

  return 0;
}

int
main (int argc, char **argv)
{
  routerd_main_t *rm = &routerd_main;
  unformat_input_t _argv, *a = &_argv;
  clib_mem_init_thread_safe (0, 256 << 20);

  setup_signal_handlers ();
  if (connect_to_vpp ("slankdev") < 0) {
    svm_region_exit ();
    fformat (stderr, "Couldn't connect to vpe, exiting...\n");
    exit (1);
  }

  send_ping(find_msg_id(CONTROL_PING_MESSAGE), 1);
  sleep(1);

  printf("waiting 100s\n");
  sleep(100);
  vl_client_disconnect_from_vlib ();
  printf("disconnected\n");
  exit (0);
}

