#define _GNU_SOURCE /* for strcasestr(3) */

#include <stdio.h>

#include <vnet/vnet.h>
#include <vppinfra/types.h>
#include <vlibapi/api.h>
#include <vlibmemory/api.h>

#define vl_api_version(n,v) static u32 vpe_api_version = (v);
#include <vpp/api/vpe.api.h>
#undef vl_api_version

#include <vpp/api/vpe_msg_enum.h>
#define vl_typedefs
#include <vpp/api/vpe_all_api_h.h>
#undef vl_typedefs

// Automatically generate endian handlers for messages
#define vl_endianfun
#include <vpp/api/vpe_all_api_h.h>
#undef vl_endianfun

// Automatically generate print handlers for messages
#define vl_print(handle, ...)
#define vl_printfun
#include <vpp/api/vpe_all_api_h.h>
#undef vl_printfun

vlib_main_t vlib_global_main;
vlib_main_t **vlib_mains;

// Keep pointers to API endpoint
typedef struct {
    unix_shared_memory_queue_t * vl_input_queue;
    u32 my_client_index;
} vppclient_main_t;

/* shared vppclient main structure */
vppclient_main_t vppclient_main __attribute__((aligned (64)));

#define CONTROL_PING_MESSAGE "control_ping"
#define CONTROL_PING_REPLY_MESSAGE "control_ping_reply"

#define CREATE_LOOPBACK_MESSAGE "create_loopback"
#define CREATE_LOOPBACK_REPLY_MESSAGE "create_loopback_reply"

#define DUMP_IFC_MESSAGE "sw_interface_dump"
#define IFC_DETAIL_MESSAGE "sw_interface_details"

#define CREATTE_BD_MESSAGE "bridge_domain_add_del"
#define CREATE_BD_REPLY_MESSAGE "bridge_domain_add_del_reply"

#define DUMP_BD_MESSAGE "bridge_domain_dump"
#define BD_DETAIL_MESSAGE "bridge_domain_details"

static u32 find_msg_id(char* msg) {
    api_main_t * am = &api_main;
    hash_pair_t *hp;

    hash_foreach_pair (hp, am->msg_index_by_name_and_crc,
    ({
        char *key = (char *)hp->key; // key format: name_crc
        int msg_name_len = strlen(key) - 9; // ignore crc
        if (strlen(msg) == msg_name_len &&
        strncmp(msg, (char *)hp->key, msg_name_len) == 0) {
            return (u32)hp->value[0];
        }
    }));
}

static int send_ping(u32 ping_id, u32 message_id) {
    vl_api_control_ping_t * mp;
    vppclient_main_t * jm = &vppclient_main;

    // Allocate control ping message
    mp = vl_msg_api_alloc(sizeof(*mp));
    memset(mp, 0, sizeof(*mp));
    // Set the message ID to control_ping ID reported by VPP
    mp->_vl_msg_id = ntohs(ping_id);
    // Set our client index
    mp->client_index = jm->my_client_index;
    // Set context (context is an arbitrary number that can help matching request/reply pairs)
    mp->context = clib_host_to_net_u32(message_id);

    // send messagee to VPP
    vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
    printf("Sending ping. Ping msg id: %d, message id: %d\n", ping_id, message_id);
}

static void vl_api_control_ping_reply_t_handler(
    vl_api_control_ping_reply_t * mp) {
    printf("\tPing reply received, message id: %d, with retval: %d\n", clib_net_to_host_u32(mp->context), mp->retval);
}

/*
        ["create_loopback",
            ["u16", "_vl_msg_id"],
            ["u32", "client_index"],
            ["u32", "context"],
            ["u8", "mac_address", 6],
            {"crc" : "0xb2602de5"}
        ],

*/
static int create_loopback(u32 create_loop_id, u32 message_id) {
    vl_api_create_loopback_t * mp;
    vppclient_main_t * jm = &vppclient_main;

    // Allocate create loopback message
    mp = vl_msg_api_alloc(sizeof(*mp));
    memset(mp, 0, sizeof(*mp));
    // Set the message ID to create_loopback ID reported by VPP
    mp->_vl_msg_id = ntohs(create_loop_id);
    // Set our client index
    mp->client_index = jm->my_client_index;
    // Set context (context is an arbitrary number that can help matching request/reply pairs)
    mp->context = clib_host_to_net_u32(message_id);
    // We could also set mac address

    // send messagee to VPP
    vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
    printf("Sending create loopback. Create loopback msg id: %d, message id: %d\n", create_loop_id, message_id);
}

/*
        ["create_loopback_reply",
            ["u16", "_vl_msg_id"],
            ["u32", "context"],
            ["i32", "retval"],
            ["u32", "sw_if_index"],
            {"crc" : "0x9520f804"}
        ],

*/
static void vl_api_create_loopback_reply_t_handler(
    vl_api_create_loopback_reply_t * mp) {
    printf("\tCreate loopback reply received, message id: %d, with retval: %d, new interface index: %d\n",
           clib_net_to_host_u32(mp->context), mp->retval, clib_net_to_host_u32(mp->sw_if_index));
}

/*
        ["sw_interface_dump",
            ["u16", "_vl_msg_id"],
            ["u32", "client_index"],
            ["u32", "context"],
            ["u8", "name_filter_valid"],
            ["u8", "name_filter", 49],
            {"crc" : "0x9a2f9d4d"}
        ],
*/
static int dump_ifcs(u32 dump_id, u32 message_id) {
    vl_api_sw_interface_dump_t * mp;
    vppclient_main_t * jm = &vppclient_main;

    // Allocate create loopback message
    mp = vl_msg_api_alloc(sizeof(*mp));
    memset(mp, 0, sizeof(*mp));
    // Set the message ID to dump_interfaces ID reported by VPP
    mp->_vl_msg_id = ntohs(dump_id);
    // Set our client index
    mp->client_index = jm->my_client_index;
    // Set context (context is an arbitrary number that can help matching request/reply pairs)
    mp->context = clib_host_to_net_u32(message_id);
    // We could also set name_filter

    // send messagee to VPP
    vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
    printf("Sending dump interfaces. Msg id: %d, message id: %d\n", dump_id, message_id);
}

/*
 ["sw_interface_details",
            ["u16", "_vl_msg_id"],
            ["u32", "context"],
            ["u32", "sw_if_index"],
            ["u32", "sup_sw_if_index"],
            ["u32", "l2_address_length"],
            ["u8", "l2_address", 8],
            ["u8", "interface_name", 64],
            ["u8", "admin_up_down"],
            ["u8", "link_up_down"],
            ["u8", "link_duplex"],
            ["u8", "link_speed"],
            ["u16", "link_mtu"],
            ["u32", "sub_id"],
            ["u8", "sub_dot1ad"],
            ["u8", "sub_dot1ah"],
            ["u8", "sub_number_of_tags"],
            ["u16", "sub_outer_vlan_id"],
            ["u16", "sub_inner_vlan_id"],
            ["u8", "sub_exact_match"],
            ["u8", "sub_default"],
            ["u8", "sub_outer_vlan_id_any"],
            ["u8", "sub_inner_vlan_id_any"],
            ["u32", "vtr_op"],
            ["u32", "vtr_push_dot1q"],
            ["u32", "vtr_tag1"],
            ["u32", "vtr_tag2"],
            ["u8", "tag", 64],
            ["u16", "outer_tag"],
            ["u8", "b_dmac", 6],
            ["u8", "b_smac", 6],
            ["u16", "b_vlanid"],
            ["u32", "i_sid"],
            {"crc" : "0xe2d855bb"}
        ],
*/
static void vl_api_sw_interface_details_t_handler(
    vl_api_sw_interface_details_t * mp) {
    printf("\tInterface, message id: %d, interface index: %d\n",
           clib_net_to_host_u32(mp->context), clib_net_to_host_u32(mp->sw_if_index));
}

/*
        ["bridge_domain_add_del",
            ["u16", "_vl_msg_id"],
            ["u32", "client_index"],
            ["u32", "context"],
            ["u32", "bd_id"],
            ["u8", "flood"],
            ["u8", "uu_flood"],
            ["u8", "forward"],
            ["u8", "learn"],
            ["u8", "arp_term"],
            ["u8", "mac_age"],
            ["u8", "bd_tag", 64],
            ["u8", "is_add"],
            {"crc" : "0xe9894b51"}
        ],
*/
static int create_bd(u32 create_bd, u32 message_id, u32 bd_id) {
    vl_api_bridge_domain_add_del_t * mp;
    vppclient_main_t * jm = &vppclient_main;

    // Allocate create loopback message
    mp = vl_msg_api_alloc(sizeof(*mp));
    memset(mp, 0, sizeof(*mp));
    // Set the message ID to create_loopback ID reported by VPP
    mp->_vl_msg_id = ntohs(create_bd);
    // Set our client index
    mp->client_index = jm->my_client_index;
    // Set context (context is an arbitrary number that can help matching request/reply pairs)
    mp->context = clib_host_to_net_u32(message_id);
    // Set bd ID
    mp->bd_id = clib_host_to_net_u32(bd_id);
    // Mark the request as ADD
    mp->is_add = 1;

    // send messagee to VPP
    vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
    printf("Sending create bridge_domain. Create bridge_domain msg id: %d, message id: %d\n", create_bd, message_id);
}

/*
        ["create_loopback_reply",
            ["u16", "_vl_msg_id"],
            ["u32", "context"],
            ["i32", "retval"],
            ["u32", "sw_if_index"],
            {"crc" : "0x9520f804"}
        ],

*/
static void vl_api_bridge_domain_add_del_reply_t_handler(
    vl_api_bridge_domain_add_del_reply_t * mp) {
    printf("\tCreate bridge_domain reply received, message id: %d, with retval: %d\n",
           clib_net_to_host_u32(mp->context), mp->retval);
}

/*
        ["bridge_domain_dump",
            ["u16", "_vl_msg_id"],
            ["u32", "client_index"],
            ["u32", "context"],
            ["u32", "bd_id"],
            {"crc" : "0x68d5401d"}
        ],
*/
static int dump_bd(u32 dump_id, u32 message_id) {
    vl_api_bridge_domain_dump_t * mp;
    vppclient_main_t * jm = &vppclient_main;

    // Allocate create loopback message
    mp = vl_msg_api_alloc(sizeof(*mp));
    memset(mp, 0, sizeof(*mp));
    // Set the message ID to dump_interfaces ID reported by VPP
    mp->_vl_msg_id = ntohs(dump_id);
    // Set our client index
    mp->client_index = jm->my_client_index;
    // Set context (context is an arbitrary number that can help matching request/reply pairs)
    mp->context = clib_host_to_net_u32(message_id);
    // Set bd_id to ~0. ~0 means dump all bridge domains. We could use a specific bd_id to dump a single bd
    mp->bd_id = ~0;

    // send messagee to VPP
    vl_msg_api_send_shmem(jm->vl_input_queue, (u8 *) &mp);
    printf("Sending dump bd. Msg id: %d, message id: %d\n", dump_id, message_id);
}

/*
 ["bridge_domain_details",
            ["u16", "_vl_msg_id"],
            ["u32", "context"],
            ["u32", "bd_id"],
            ["u8", "flood"],
            ["u8", "uu_flood"],
            ["u8", "forward"],
            ["u8", "learn"],
            ["u8", "arp_term"],
            ["u8", "mac_age"],
            ["u8", "bd_tag", 64],
            ["u32", "bvi_sw_if_index"],
            ["u32", "n_sw_ifs"],
            ["vl_api_bridge_domain_sw_if_t", "sw_if_details", 0, "n_sw_ifs"],
            {"crc" : "0xff7d2b54"}
        ],
*/
static void vl_api_bridge_domain_details_t_handler(
    vl_api_bridge_domain_details_t * mp) {
    printf("\tBridge_domain, message id: %d, bd index: %d\n",
           clib_net_to_host_u32(mp->context), clib_net_to_host_u32(mp->bd_id));
}
// endian and print handlers for bd_details were not generated, so just set then to noop handler for now
#define vl_api_bridge_domain_details_t_endian vl_noop_handler
#define vl_api_bridge_domain_details_t_print vl_noop_handler

int main()
{
    vppclient_main_t * jm = &vppclient_main;
    api_main_t * am = &api_main;

    // Open VPP management session under name vpp-manager
    if (vl_client_connect_to_vlib("vpe-api", "vpp-manager", 32) < 0)
        return -1;

    printf("Connected to VPP! as client: %d\n", jm->my_client_index);

    jm->my_client_index = am->my_client_index;
    jm->vl_input_queue = am->shmem_hdr->vl_input_queue;

    // Register ping reply handler
    vl_msg_api_set_handlers(find_msg_id(CONTROL_PING_REPLY_MESSAGE), CONTROL_PING_REPLY_MESSAGE,
                            vl_api_control_ping_reply_t_handler, vl_noop_handler,
                            vl_api_control_ping_reply_t_endian,
                            vl_api_control_ping_reply_t_print,
                            sizeof(vl_api_control_ping_reply_t), 1);

    // Register dump_interfaces reply handler
    vl_msg_api_set_handlers(find_msg_id(IFC_DETAIL_MESSAGE), IFC_DETAIL_MESSAGE,
                            vl_api_sw_interface_details_t_handler, vl_noop_handler,
                            vl_api_sw_interface_details_t_endian,
                            vl_api_sw_interface_details_t_print,
                            sizeof(vl_api_sw_interface_details_t), 1);

    // Register create_loopback reply handler
    vl_msg_api_set_handlers(find_msg_id(CREATE_LOOPBACK_REPLY_MESSAGE), CREATE_LOOPBACK_REPLY_MESSAGE,
                            vl_api_create_loopback_reply_t_handler, vl_noop_handler,
                            vl_api_create_loopback_reply_t_endian,
                            vl_api_create_loopback_reply_t_print,
                            sizeof(vl_api_create_loopback_reply_t), 1);

    // Register create_loopback reply handler
    vl_msg_api_set_handlers(find_msg_id(CREATE_BD_REPLY_MESSAGE), CREATE_BD_REPLY_MESSAGE,
                            vl_api_bridge_domain_add_del_reply_t_handler, vl_noop_handler,
                            vl_api_bridge_domain_add_del_reply_t_endian,
                            vl_api_bridge_domain_add_del_reply_t_print,
                            sizeof(vl_api_bridge_domain_add_del_reply_t), 1);

    // Register dump_bridge_domain reply handler
    vl_msg_api_set_handlers(find_msg_id(BD_DETAIL_MESSAGE), BD_DETAIL_MESSAGE,
                            vl_api_bridge_domain_details_t_handler, vl_noop_handler,
                            vl_api_bridge_domain_details_t_endian,
                            vl_api_bridge_domain_details_t_print,
                            sizeof(vl_api_bridge_domain_details_t), 1);

    // Send initial ping to check all works
    send_ping(find_msg_id(CONTROL_PING_MESSAGE), 1);
    sleep(1);
    // Show interfaces
    dump_ifcs(find_msg_id(DUMP_IFC_MESSAGE), 2);
    sleep(1);
    // Create Loopback interface
    create_loopback(find_msg_id(CREATE_LOOPBACK_MESSAGE), 3);
    sleep(1);
    // Show interfaces
    dump_ifcs(find_msg_id(DUMP_IFC_MESSAGE), 4);
    sleep(1);
    // Show bridge domains
    dump_bd(find_msg_id(DUMP_BD_MESSAGE), 5);
    sleep(1);
    // Add bridge domain 99
    create_bd(find_msg_id(CREATTE_BD_MESSAGE), 6, 99);
    sleep(1);
    // Show bridge domains
    dump_bd(find_msg_id(DUMP_BD_MESSAGE), 7);
    sleep(1);

    // TODO Now add interface to BD 

    return 0;
}
