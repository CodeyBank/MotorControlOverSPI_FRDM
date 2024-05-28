/*
 * se_udp_communication.c
 *
 *  Created on: Mar 8, 2024
 *      Author: Stanley.Ezeh
 */


#include "se_udp_communication.h"
#include "NCN26010.h"
#include "onsemi_hardware.h"
#include "T1S_TCP-IP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "stdio.h"
#include <stdlib.h>
#include "FreeRTOS_errno_TCP.h"
#include"string.h"
//#define interface_test

typedef struct freertos_sockaddr freertos_sockaddr_t;


// PRIVATE FUNCTION PROTOTYPES

static uint32_t _se_udp_comm_recv(uint8_t* msg_buff, uint32_t max_msg_sz, SE_Net_Addr_V4_t *sender_addr);
static uint32_t _se_udp_comm_send(SE_Net_Addr_V4_t * dest_addr, uint8_t * msg_buff, uint32_t msg_size);
static void _se_udp_comm_sock_close(void);
static void _se_net_addr_to_uint32(SE_Net_Addr_V4_t *se_addr, freertos_sockaddr_t *sockaddr);
static void _sockaddr_to_se_net_addr(freertos_sockaddr_t * sockaddr, SE_Net_Addr_V4_t *se_addr);

// PRIVATE VARIABLES
static Socket_t se_udp_socket;
static struct freertos_sockaddr xLocalAddress;

extern uint8_t active_connection;


uint32_t udp_comm_send(SE_Net_Addr_V4_t * dest_addr, uint8_t * msg_buff, uint8_t msg_size){

    return  _se_udp_comm_send(dest_addr, msg_buff, msg_size);

}

// only copy function pointer addresses to expose the api to the user at higher level
error_t se_udp_comm_init(SE_UDP_NET_DRV_V4_t *udp_drv)
{
	//if () check that IP address and port are set
	assert(udp_drv->ipconfig.ipv4_addr != NULL);

	// copy the ipadrress // please test
	/* Set up the local address. */
	memcpy((void *)&xLocalAddress.sin_addr, (void *)udp_drv->ipconfig.ipv4_addr , 4);
	xLocalAddress.sin_port = FreeRTOS_htons( udp_drv->ipconfig.port );
	//	xLocalAddress.sin_addr = FreeRTOS_inet_addr( "0.0.0.0" ); // Bind to all interfaces

	BaseType_t err;
	const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( SE_NET_BLOCKING_TIMEOUT );

    /* Create new Socket */
    se_udp_socket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );

    if (se_udp_socket == FREERTOS_INVALID_SOCKET )
    {
        return FAIL;
    }


	/* Bind address and port */
	err = FreeRTOS_bind( se_udp_socket, &xLocalAddress, sizeof( xLocalAddress ) );
	FreeRTOS_setsockopt( se_udp_socket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
	char ip[16];
	FreeRTOS_inet_ntoa(xLocalAddress.sin_addr, ip);
    if (err != 0)
    {
    	// close socket if binding failure
    	FreeRTOS_closesocket( se_udp_socket );
    	PRINTF("Failed to bind to socketIP:%s and Port %d \r\n", ip, FreeRTOS_ntohs(xLocalAddress.sin_port));
        return FAIL;
    }

    PRINTF("UDP socket created and bound to IP:%s and Port %d \r\n", ip, FreeRTOS_ntohs(xLocalAddress.sin_port));

    // set up the connection interface
    // store the connection details to the struct
    // address data structure for ipV4

    udp_drv->Close = _se_udp_comm_sock_close;
    udp_drv->Recv = _se_udp_comm_recv;
    udp_drv->Send = _se_udp_comm_send;

    return SUCCESS;
}

static uint32_t _se_udp_comm_recv(uint8_t* msg_buff, uint32_t max_msg_sz, SE_Net_Addr_V4_t *sender_addr)
{
    long recvSize = 0;
    freertos_sockaddr_t xClientAddress;  //use this to store the details of the sender
    socklen_t xSize = sizeof(xClientAddress);

    assert(msg_buff != NULL);

    memset(( void * )msg_buff, 0, max_msg_sz);


    if (se_udp_socket == NULL)
    {
        PRINTF("Attempting to call receive UDP receive with a closed or uninitialized socket\n");
    	return 0;
    }

    // Receive data from a client
    recvSize = FreeRTOS_recvfrom(se_udp_socket, (void *)msg_buff, max_msg_sz, 0, &xClientAddress, &xSize);

	if (recvSize > 0) {
		// message received
		char ip[16];
		FreeRTOS_inet_ntoa(xClientAddress.sin_addr, ip);

		if(!active_connection){
			// set the active connection IP
//			_uint32_to_ipv4_array(xClientAddress.sin_addr, sender_addr);
			_sockaddr_to_se_net_addr(&xClientAddress, sender_addr);
			active_connection = 1;
		}
#ifdef interface_test
		PRINTF("Received %d bytes from %s on port %d: %s\n", recvSize, ip, FreeRTOS_ntohs(xClientAddress.sin_port), msg_buff);
		int i=0;
	    while(i<recvSize){
	    	PRINTF("0x%X", msg_buff[i]);
	    	i++;
	    }
#endif // interface_test
	}
	else{
		return 0; // if nothing is received
	}
    return recvSize;
}

void _uint32_to_ipv4_array(uint32_t xSocket_addr, uint8_t ipv4_array[4])
{
    ipv4_array[3] = (xSocket_addr >> 24) & 0xFF; // Extract the most significant byte
    ipv4_array[2] = (xSocket_addr >> 16) & 0xFF;
    ipv4_array[1] = (xSocket_addr >> 8) & 0xFF;
    ipv4_array[0] = xSocket_addr & 0xFF;         // Extract the least significant byte
}

static void _sockaddr_to_se_net_addr(freertos_sockaddr_t * sockaddr, SE_Net_Addr_V4_t *se_addr){
	se_addr->sender_port = FreeRTOS_ntohs(sockaddr->sin_port);
	// copy the ip
	_uint32_to_ipv4_array(sockaddr->sin_addr, se_addr->sender_addr);
}


static uint32_t _se_udp_comm_send(SE_Net_Addr_V4_t * dest_addr, uint8_t * msg_buff, uint32_t msg_size)
{
    uint32_t sentSize = 0;
    freertos_sockaddr_t destinationAddr;

    assert(msg_buff != NULL);
    assert(dest_addr != NULL);

    if (se_udp_socket == NULL)
    {
        sentSize = -1;
    }

    /* Copy destination address */
    _se_net_addr_to_uint32(dest_addr, &destinationAddr);


#ifdef interface_test
    PRINTF("sending %s ", msg_buff);
    PRINTF("Message size = %d\n", msg_size);
	char ip[16];
	FreeRTOS_inet_ntoa(destinationAddr.sin_addr, ip);
    PRINTF("Destination addr: %s and port %d \n", ip, FreeRTOS_htons(destinationAddr.sin_port));
#endif // interface_test
    /* Send data */
	sentSize = (uint32_t)FreeRTOS_sendto( se_udp_socket,				/* The socket being sent to. */
					( void * ) msg_buff,	/* The data being sent. */
					msg_size,/* The length of the data being sent. */
					0,						/* ulFlags with the FREERTOS_ZERO_COPY bit clear. */
					&destinationAddr,	/* The destination address. */
					sizeof( destinationAddr ) );

    if (sentSize<0)
    {
    	PRINTF("Error occurred in datagram transmission \n");
    }
    return sentSize;
}

static void _se_udp_comm_sock_close(void)
{
	if (FreeRTOS_closesocket(se_udp_socket) ==1)
		PRINTF("Successfully closed socket \n");
	else
		{
			PRINTF("Failed to close socket \n");
		}
}

static void _se_net_addr_to_uint32(SE_Net_Addr_V4_t *se_addr, freertos_sockaddr_t *sockaddr)
{
	// Clear the sin_addr field before setting it
	sockaddr->sin_addr = 0;
	sockaddr->sin_addr  |= (se_addr->sender_addr[3] << 24);
	sockaddr->sin_addr  |= (se_addr->sender_addr[2] << 16);
	sockaddr->sin_addr  |= (se_addr->sender_addr[1] << 8);
	sockaddr->sin_addr  |= se_addr->sender_addr[0];

    // Set the port number
	sockaddr->sin_port = FreeRTOS_htons(se_addr->sender_port);

}

void ipconfig(SE_UDP_NET_DRV_V4_t * udp_drv) {
	//void // todo implement later
	char ip[16];
	uint32_t addr_int = 0;
	addr_int |= udp_drv->ipconfig.ipv4_addr[0]
			| udp_drv->ipconfig.ipv4_addr[1] << 8
			| udp_drv->ipconfig.ipv4_addr[2] << 16
			| udp_drv->ipconfig.ipv4_addr[3] << 24;

	FreeRTOS_inet_ntoa(addr_int, ip);
	PRINTF("\n\n***** UDP INTERFACE DRIVER CONFIGURATION ***** \n");
	PRINTF("\t\tInterface Name %s \n", udp_drv->ipconfig.dn);
	PRINTF("\t\tIPV4 ADDRESS: %s\n", ip);
	PRINTF("\t\tPORT: %d\n", udp_drv->ipconfig.port);
	PRINTF("\t\tSOCKET STATUS: %s\n", se_udp_socket == NULL? "DOWN" : "UP");
	PRINTF("**************** END ************************ \n\n");
}

