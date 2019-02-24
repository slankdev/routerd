#pragma once

#include <json.h>
#include <vector>
#include <string>
#include <slankdev/filefd.h>
#include "log.h"

struct conf {
  bool debug_trace;
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
  static conf& get_instance()
  {
    static conf instance;
    return instance;
  }
  void load_file(const char* path)
  {
    if (path) {
      log_info("config.json was found\n");
      json_str = file2str(path);
      json_object* root = json_tokener_parse(json_str.c_str());

      /* debug_trace */
      json_object* jo3;
      json_bool ret3 = json_object_object_get_ex(root, "debug_trace", &jo3);
      if (ret3) {
        const bool b = json_object_get_boolean(jo3);
        debug_trace = b;
      }
    }
  }
 private:
  conf() : debug_trace(false) {}
 public:
  void dump(FILE* fp) const
  {
    fprintf(fp, "conf->debug_trace: %s\r\n", debug_trace?"true":"false");
  }
};
