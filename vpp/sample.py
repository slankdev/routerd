#!/usr/bin/env python3

from __future__ import print_function

import os
import time
import fnmatch
from vpp_papi import VPP

vpp_json_dir = '/usr/share/vpp/api/core/'
jsonfiles = []
for root, dirnames, filenames in os.walk(vpp_json_dir):
    for filename in fnmatch.filter(filenames, '*.api.json'):
        jsonfiles.append(os.path.join(vpp_json_dir, filename))

if not jsonfiles:
    print('Error: no json api files found')
    exit(-1)

vpp = VPP(jsonfiles)
r = vpp.connect("test_papi")
print(r)

print('sleep')
time.sleep(10)
print('sleep ... done')

for intf in vpp.api.sw_interface_dump():
    print(intf.interface_name.decode())
vpp.disconnect()
exit(0)
