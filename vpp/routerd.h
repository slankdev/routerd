#ifndef _ROUTERD_H_
#define _ROUTERD_H_

#include <stdio.h>
#include <vector>
#include <string>

struct routerd_interface {
  uint32_t kern_ifindex;
  uint32_t vpp_ifindex;
  routerd_interface(uint32_t k, uint32_t v) :
    kern_ifindex(k), vpp_ifindex(v) {}
};

struct routerd_context {
  std::vector<struct routerd_interface> interfaces;

  struct {
    bool enable_interactive;
    std::string config_path;
  } global_config;

  routerd_context()
  {
    global_config.enable_interactive = false;
    global_config.config_path = "/etc/routerd/routerd.conf";
  }

  void add_interface(uint32_t k, uint32_t v)
  {
    routerd_interface iface(k, v);
    interfaces.push_back(iface);
  }

  void dump(FILE *fp) const
  {
    fprintf(fp, "==========================================\r\n");
    fprintf(fp, "global_config.enable_interactive: %s\r\n",
        global_config.enable_interactive ? "true" : "false");
    fprintf(fp, "global_config.config_path: %s\r\n",
        global_config.config_path.c_str());
    fprintf(fp, "==========================================\r\n");
  }
};

extern struct routerd_context rd_ctx;

#endif /* _ROUTERD_H_ */
