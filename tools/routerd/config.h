#pragma once

#include <json.h>
#include <vector>
#include <string>
#include <slankdev/filefd.h>
#include "log.h"

struct conf {
  std::vector<uint32_t> ignore_devices;
  bool debug;
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
  conf(const char* path)
  {
    if (path) {
      log_info("config.json was found\n");
      std::string json_str = file2str(path);
      printf("%s", json_str.c_str());
    }
  }
};
