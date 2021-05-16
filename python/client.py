#!/usr/bin/env python3

from __future__ import print_function
import os
import fnmatch
from vpp_papi import VPPApiJSONFiles
from vpp_papi import vpp_papi

CLIENT_ID = "Vppclient"
VPP_JSON_DIR = '/usr/share/vpp/api/core/'
API_FILE_SUFFIX = '*.api.json'


def load_json_api_files(json_dir=VPP_JSON_DIR, suffix=API_FILE_SUFFIX):
    jsonfiles = []
    for root, dirnames, filenames in os.walk(json_dir):
        for filename in fnmatch.filter(filenames, suffix):
            jsonfiles.append(os.path.join(json_dir, filename))

    if not jsonfiles:
        print('Error: no json api files found')
        exit(-1)

    return jsonfiles


def connect_vpp(jsonfiles):
    vpp = vpp_papi.VPPApiClient(apifiles=jsonfiles)
    r = vpp.connect("CLIENT_ID")
    print("VPP api opened with code: %s" % r)
    return vpp


def dump_interfaces():
    print("Sending dump interfaces. Msg id: sw_interface_dump")
    for intf in vpp.api.sw_interface_dump():
        print("\tInterface, message id: sw_interface_details, interface index: %s" % intf.interface_name.decode())


def dump_bds():
    print("Sending dump bridge domains. Msg id: bridge_domain_dump")
    for intf in vpp.api.bridge_domain_dump(bd_id = int("ffffffff", 16)):
        print("\tBridge domain, message id: bridge_domain_details, bd index: %s" % intf.bd_id)


def create_loopback():
    print("Sending create loopback. Msg id: create_loopback_interface")
    vpp.api.create_loopback()


def create_bd():
    print("Sending create loopback. Msg id: create_loopback_interface")
    vpp.api.bridge_domain_add_del(is_add = 1, bd_id = 99)


# Python apis need json definitions to interpret messages
vpp = connect_vpp(load_json_api_files())
# Dump interfaces
"""
dump_interfaces()
This function is not working at the moment feel free to correct the code
"""
# Create loopback
create_loopback()
# Dump interfaces
dump_interfaces()
# Dump bridge-domains
dump_bds()
# Create bridge domain
create_bd()
# Dump bridge-domains
dump_bds()

exit(vpp.disconnect())
