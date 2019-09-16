#ifndef _NETLINK_H_
#define _NETLINK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <yalin/yalin.h>

#ifdef __cplusplus
extern "C" {
#endif

struct netlink_counter {
  public:
    struct {
      size_t new_cnt;
      size_t del_cnt;
    } link;
    struct {
      size_t new_cnt;
      size_t del_cnt;
    } addr;
    struct {
      size_t new_cnt;
      size_t del_cnt;
    } route;
    struct {
      size_t new_cnt;
      size_t del_cnt;
    } neigh;
  public:
    netlink_counter() {}
    virtual ~netlink_counter() {}
    size_t link_all() const { return link.new_cnt + link.del_cnt; }
    size_t addr_all() const { return addr.new_cnt + addr.del_cnt; }
    size_t route_all() const { return route.new_cnt + route.del_cnt; }
    size_t neigh_all() const { return neigh.new_cnt + neigh.del_cnt; }
};

extern netlink_counter counter;
extern netlink_cache_t *nlc;

void netlink_manager();
bool netlink_monitor_is_enable();

#ifdef __cplusplus
}
#endif
#endif /* _NETLINK_H_ */
