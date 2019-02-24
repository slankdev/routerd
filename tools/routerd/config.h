#pragma once

#include <json.h>
#include <vector>
#include <string>
#include <slankdev/filefd.h>
#include "log.h"

struct conf {
  std::vector<uint32_t> ignore_devices;
  bool debug;
  bool debug_iproute2_cli;
  std::string json_str;
  static std::string file2str(const char* path)
  {
    slankdev::filefd file;
    file.fopen(path, "r");
    std::string str;
    while (true) {
      char buf[1];
      int ret = file.fread(buf, sizeof(buf), 1);
      if (ret <= 0) break;
      str.push_back(buf[0]);
    }
    return str;
  }
  conf(const char* path) : debug(false), debug_iproute2_cli(false)
  {
    if (path) {
      log_info("config.json was found\n");
      json_str = file2str(path);
      json_object* root = json_tokener_parse(json_str.c_str());

      /* debug */
      json_object* jo1;
      json_bool ret1 = json_object_object_get_ex(root, "debug", &jo1);
      if (ret1) {
        const bool b = json_object_get_boolean(jo1);
        debug = b;
      }

      /* debug_iproute2_cli */
      json_object* jo3;
      json_bool ret3 = json_object_object_get_ex(root, "debug_iproute2_cli", &jo3);
      if (ret3) {
        const bool b = json_object_get_boolean(jo3);
        debug_iproute2_cli = b;
      }

      /* ignore_devices */
      json_object* jo2;
      json_bool ret2 = json_object_object_get_ex(root, "ignore_devices", &jo2);
      if (ret2) {
        const size_t n_devs = json_object_array_length(jo2);
        ignore_devices.resize(n_devs);
        for (size_t i=0; i<n_devs; i++) {
          json_object* jo = json_object_array_get_idx(jo2, i);
          const size_t index = json_object_get_int(jo);
          ignore_devices[i] = index;
        }
      }
    }
  }
  void dump(FILE* fp) const
  {
    fprintf(fp, "conf->debug: %s\r\n", debug?"true":"false");
    fprintf(fp, "conf->debug_iproute2_cli: %s\r\n", debug_iproute2_cli?"true":"false");
    fprintf(fp, "conf->ignore_devices: [");
    for (size_t i=0; i<ignore_devices.size(); i++)
      fprintf(fp, "%u%s", ignore_devices[i],
          i+1<ignore_devices.size()?", ":"]\r\n");
  }
};
