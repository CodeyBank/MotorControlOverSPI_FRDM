/*
 * se_udp_communication.h
 *
 *  Created on: Mar 8, 2024
 *      Author: Stanley.Ezeh
 */

#ifndef INCLUDE_SE_UDP_COMMUNICATION_H_
#define INCLUDE_SE_UDP_COMMUNICATION_H_
#include "stdint.h"
#include "onsemi_hardware.h"

#define SE_UDP_PORT  3454
#define DOMAIN_NAME_LENGTH 12
#define SE_NET_BLOCKING_TIMEOUT 10
// address data structure for ipV4
typedef struct SE_Net_Addr_S
{
	uint16_t port;
    uint8_t ipv4_addr[4];
    uint8_t sender_addr[4]; // don't set this. will be automatically set
    uint16_t sender_port;
    char dn [DOMAIN_NAME_LENGTH]; // fancy domain name must be short
} SE_Net_Addr_V4_t;


// udp driver object for exporting functions.
// TODO provide functionality to instantiate more than one of these to be used
typedef struct UDP_DRV
{
    uint32_t (*Recv)(uint8_t* msg_buff, uint32_t max_msg_sz, SE_Net_Addr_V4_t *sender_addr);  /* Receive data */
    uint32_t (*Send)(SE_Net_Addr_V4_t * dest_addr, uint8_t * msg_buff, uint32_t msg_size);     /* Send data */
    void (*Close)(void); /* Close active session */
    SE_Net_Addr_V4_t ipconfig;  // give user details of network driver object
} SE_UDP_NET_DRV_V4_t;


error_t se_udp_comm_init(SE_UDP_NET_DRV_V4_t *udp_drv);
void ipconfig(SE_UDP_NET_DRV_V4_t * udp_drv) ;
uint32_t udp_comm_send(SE_Net_Addr_V4_t * dest_addr, uint8_t * msg_buff, uint8_t msg_size);

//
void _uint32_to_ipv4_array(uint32_t xSocket_addr, uint8_t ipv4_array[4]);
#endif /* INCLUDE_SE_UDP_COMMUNICATION_H_ */
