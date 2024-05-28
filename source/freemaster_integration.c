/*
 * freemaster_integration.c
 *
 *  Created on: Feb 19, 2024
 *      Author: Stanley.Ezeh
 */

#include "freemaster.h"
#include "freemaster_net.h"
#include "freemaster_protocol.h"
#include "freemaster_private.h"

#include "freemaster_integration.h"
#include "NCN26010.h"
#include "onsemi_hardware.h"
#include "T1S_TCP-IP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "stdio.h"
#include <stdlib.h>
#include "FreeRTOS_errno_TCP.h"


typedef struct freertos_sockaddr freertos_sockaddr_t;

FMSTR_BOOL _FMSTR_NetOnsemiUdpInit(void);
void _FMSTR_NetOnsemiUdpPoll(void);
FMSTR_S32 _FMSTR_NetOnsemiUdpRecv(FMSTR_BPTR msgBuff,
                                       FMSTR_SIZE msgMaxSize,
                                       FMSTR_NET_ADDR *recvAddr,
                                       FMSTR_BOOL *isBroadcast);
FMSTR_S32 _FMSTR_NetOnsemiUdpSend(FMSTR_NET_ADDR *sendAddr, FMSTR_BPTR msgBuff, FMSTR_SIZE msgSize);
void _FMSTR_NetOnsemiUdpClose(FMSTR_NET_ADDR *addr);
void _FMSTR_NetOnsemiUdpGetCaps(FMSTR_NET_IF_CAPS *caps);


void _FMSTR_NetAddrFromFmstr(FMSTR_NET_ADDR *fmstrAddr, struct freertos_sockaddr *xSocketAddr);
static void uint32_to_ipv4_array(uint32_t xSocket_addr, uint8_t ipv4_array[4]);
static freertos_sockaddr_t FMSTR_NET_ADDR_to_uint32(FMSTR_NET_ADDR *fmstrAddr);

/* Interface of this network UDP driver */
const FMSTR_NET_DRV_INTF FMSTR_NET_ONSEMI_UDP = {
    .Init    = _FMSTR_NetOnsemiUdpInit,
    .Poll    = _FMSTR_NetOnsemiUdpPoll,
    .Recv    = _FMSTR_NetOnsemiUdpRecv,
    .Send    = _FMSTR_NetOnsemiUdpSend,
    .Close   = _FMSTR_NetOnsemiUdpClose,
    .GetCaps = _FMSTR_NetOnsemiUdpGetCaps,
};

static Socket_t fmstrUdpConn;
struct freertos_sockaddr xLocalAddress;
TaskHandle_t UdpTaskHandle;

FMSTR_BOOL _FMSTR_NetOnsemiUdpInit(void)
{
	BaseType_t err;
	const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( FMSTR_NET_BLOCKING_TIMEOUT );

    /* Create new Socket */

    fmstrUdpConn = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );

    if (fmstrUdpConn == FREERTOS_INVALID_SOCKET )
    {
        return FMSTR_FALSE;
    }

	/* Set up the local address. */

	xLocalAddress.sin_port = FreeRTOS_htons( FMSTR_NET_PORT );
	xLocalAddress.sin_addr = FreeRTOS_inet_addr( "0.0.0.0" ); // Bind to all interfaces
	err = FreeRTOS_bind( fmstrUdpConn, &xLocalAddress, sizeof( xLocalAddress ) );

	/* Bind address and port */
	FreeRTOS_setsockopt( fmstrUdpConn, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );

    if (err != 0)
    {
    	FreeRTOS_closesocket( fmstrUdpConn );
        return FMSTR_FALSE;
    }else{
    	char ip[16];
    	FreeRTOS_inet_ntoa(xLocalAddress.sin_addr, ip);
    	PRINTF("UDP socket created and bound to IP:%s and Port %d \r\n", ip, FreeRTOS_ntohs(xLocalAddress.sin_port));
    }

    return FMSTR_TRUE;
}


void _FMSTR_NetOnsemiUdpPoll(void)
{
//	PRINTF("POLL\n");
}

FMSTR_S32 _FMSTR_NetOnsemiUdpRecv(FMSTR_BPTR msgBuff,
                                       FMSTR_SIZE msgMaxSize,
                                       FMSTR_NET_ADDR *recvAddr,
                                       FMSTR_BOOL *isBroadcast)
{
//	BaseType_t err;
    long recvSize = 0;
    struct freertos_sockaddr xClientAddress;
    socklen_t xSize = sizeof(xClientAddress);

    FMSTR_ASSERT(msgBuff != NULL);
    FMSTR_ASSERT(recvAddr != NULL);
    FMSTR_ASSERT(isBroadcast != NULL);

    memset(( void * )msgBuff, 0, msgMaxSize);

    *isBroadcast = FMSTR_FALSE;

    if (fmstrUdpConn == NULL)
    {
        return -1;
    }

    // Receive data from a client

    recvSize = FreeRTOS_recvfrom(fmstrUdpConn, (void *)msgBuff, msgMaxSize, 0, &xClientAddress, &xSize);
	if (recvSize > 0) {
		char ip[16];
		FreeRTOS_inet_ntoa(xClientAddress.sin_addr, ip);
#ifdef interface_test
		PRINTF("Received %d bytes from %s on port %d: %s\n", recvSize, ip, FreeRTOS_ntohs(xClientAddress.sin_port), msgBuff);

		int i=0;
	    while(i<recvSize){
	    	PRINTF("0x%x", msgBuff[i]);
	    	i++;
	    }
#endif // interface_test
	}
	else{
		return 0;
	}


    /* Copy address of receiver */

	_FMSTR_NetAddrFromFmstr(recvAddr, &xClientAddress);
#ifdef interface_test
	for(int i=0; i<4; i++){
		PRINTF("%d", recvAddr->addr.v4[i]);
		if(i<3)PRINTF(".");
	}
	PRINTF("\n");
#endif // interface_test
    return recvSize;
}


void _FMSTR_NetAddrFromFmstr(FMSTR_NET_ADDR *fmstrAddr, struct freertos_sockaddr *xSocketAddr)
{
    FMSTR_ASSERT(fmstrAddr != NULL);
    FMSTR_ASSERT(xSocketAddr != NULL);
    fmstrAddr->type = FMSTR_NET_ADDR_TYPE_V4;
	uint8_t ipv4_array[4];
	uint32_to_ipv4_array(xSocketAddr->sin_addr, ipv4_array);
    /* Port */
    fmstrAddr->port = xSocketAddr->sin_port;

    if (fmstrAddr->type == FMSTR_NET_ADDR_TYPE_V4)
    {
        /* IP address */
        FMSTR_MemCpy( fmstrAddr->addr.v4, ipv4_array, 4);
    }

}

void vUDPServerTask(void *pvParameters){
#ifdef interface_test
	_FMSTR_NetOnsemiUdpInit();
	FMSTR_SIZE msgMaxSize =256;
	unsigned char *msg[msgMaxSize];

//	FMSTR_NET_ADDR recvAddr,sendAddr;
	FMSTR_BOOL isBroadcast=FMSTR_FALSE;
	FMSTR_BPTR msgBuff = (unsigned char *)malloc(50);

	sprintf((char *)msgBuff, "Heart beat of life \n");

//	int msgSize = 50;
	sendAddr.type = FMSTR_NET_ADDR_TYPE_V4;
	sendAddr.port = 1234;
	sendAddr.addr.v4 [0] = 192;
	sendAddr.addr.v4 [1] = 1;
	sendAddr.addr.v4 [2] = 1;
	sendAddr.addr.v4 [3] = 5;


	while(1){
//		_FMSTR_NetOnsemiUdpSend(&sendAddr, msgBuff, msgSize);
		DelayMs(300);
		_FMSTR_NetOnsemiUdpRecv((FMSTR_BPTR)msg, msgMaxSize, &recvAddr, &isBroadcast);
	}
	vTaskDelay(( TickType_t ) 250 / portTICK_PERIOD_MS);
}

void start_task(void){
	xTaskCreate(vUDPServerTask, "UDPSErver", 1024, NULL, tskIDLE_PRIORITY, &UdpTaskHandle);
	if( UdpTaskHandle!= NULL){
		PRINTF("Successfully created UDP server \n");
	};
#endif // interface_test
}

static void uint32_to_ipv4_array(uint32_t xSocket_addr, uint8_t ipv4_array[4])
{
    ipv4_array[3] = (xSocket_addr >> 24) & 0xFF; // Extract the most significant byte
    ipv4_array[2] = (xSocket_addr >> 16) & 0xFF;
    ipv4_array[1] = (xSocket_addr >> 8) & 0xFF;
    ipv4_array[0] = xSocket_addr & 0xFF;         // Extract the least significant byte
}

static freertos_sockaddr_t FMSTR_NET_ADDR_to_uint32(FMSTR_NET_ADDR *fmstrAddr)
{
	freertos_sockaddr_t retval;  // alias for struct freertos_sockaddr
    // Clear the sin_addr field before setting it
    retval.sin_addr = 0;

    retval.sin_addr |= (fmstrAddr->addr.v4[3] << 24);
    retval.sin_addr  |= (fmstrAddr->addr.v4[2] << 16);
    retval.sin_addr  |= (fmstrAddr->addr.v4[1] << 8);
    retval.sin_addr  |= fmstrAddr->addr.v4[0];

    // Set the port number
    retval.sin_port = fmstrAddr->port;

	return retval;
}



FMSTR_S32 _FMSTR_NetOnsemiUdpSend(FMSTR_NET_ADDR *sendAddr, FMSTR_BPTR msgBuff, FMSTR_SIZE msgSize)
{
    FMSTR_S32 sentSize = 0;
    freertos_sockaddr_t destinationAddr;
    char errorbuffer[256];

    FMSTR_ASSERT(msgBuff != NULL);
    FMSTR_ASSERT(sendAddr != NULL);

    if (fmstrUdpConn == NULL)
    {
        sentSize = -1;
    }

    /* Copy destination address */
    destinationAddr = FMSTR_NET_ADDR_to_uint32(sendAddr);

#ifdef interface_test
    PRINTF("sending %s ", msgBuff);
    PRINTF("Message size = %d\n", msgSize);
	char ip[16];
	FreeRTOS_inet_ntoa(destinationAddr.sin_addr, ip);
    PRINTF("Destination addr: %s\n", ip);
#endif // interface_test
    /* Send data */
	sentSize = (FMSTR_S32)FreeRTOS_sendto( fmstrUdpConn,				/* The socket being sent to. */
					( void * ) msgBuff,	/* The data being sent. */
					msgSize,/* The length of the data being sent. */
					0,						/* ulFlags with the FREERTOS_ZERO_COPY bit clear. */
					&destinationAddr,	/* The destination address. */
					sizeof( destinationAddr ) );

    if (sentSize<0)
    {
    	PRINTF("Error occurred in datagram transmission: %s \n", FreeRTOS_strerror_r(sentSize, errorbuffer, 256));
    }

    return sentSize;
}

void _FMSTR_NetOnsemiUdpClose(FMSTR_NET_ADDR *addr)
{
	if (FreeRTOS_closesocket(fmstrUdpConn) ==0)
		PRINTF("Successfully closed socket \n");
	else
		{
			PRINTF("Failed to close socket \n");
		}
}

void _FMSTR_NetOnsemiUdpGetCaps(FMSTR_NET_IF_CAPS *caps)
{
    FMSTR_ASSERT(caps != NULL);

    caps->flags |= FMSTR_NET_IF_CAPS_FLAG_UDP;
}


