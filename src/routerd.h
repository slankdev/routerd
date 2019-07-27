#ifndef _ROUTERD_H_
#define _ROUTERD_H_

#include <stdio.h>
#include <vector>
#include <string>

struct routerd_interface {
  uint32_t kern_ifindex;
  uint32_t vpp_ifindex;
  std::string kern_ifname;
  std::string vpp_ifname;

  routerd_interface(uint32_t k, const char *k_name,
      uint32_t v, const char *v_name) :
    kern_ifindex(k),
    vpp_ifindex(v),
    kern_ifname(k_name),
    vpp_ifname(v_name) {}
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

  void add_interface(uint32_t k, const char *k_name,
      uint32_t v, const char *v_name)
  {
    routerd_interface iface(k, k_name, v, v_name);
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
