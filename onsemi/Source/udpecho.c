/*
 * Author: Stanley Ezeh
 * Date : 13 February 2023
 *
 * Copyright: FreeRTOS.org and Future Electronics
 * */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+UDP includes. */
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_Sockets.h"

#include "udpecho.h"


/* Small delay used between attempts to obtain a zero copy buffer. */
#define echoTINY_DELAY	( ( TickType_t ) 2 )

/* The echo tasks create a socket, send out a number of echo requests
(listening for each echo reply), then close the socket again before
starting over.  This delay is used between each iteration to ensure the
network does not get too congested. */
#define echoLOOP_DELAY	( ( TickType_t ) 250 / portTICK_RATE_MS )
#define FreeRTOS_htons( usIn )    ( ( uint16_t ) ( ( ( usIn ) << 8U ) | ( ( usIn ) >> 8U ) ) )

#if ipconfigINCLUDE_EXAMPLE_FREERTOS_PLUS_TRACE_CALLS == 1
	/* When the trace recorder code is included user events are generated to
	mark the sending and receiving of the echoed data (only in the zero copy
	task. */
	#define echoMARK_SEND_IN_TRACE_BUFFER( x ) vTraceUserEvent( x )
	traceLabel xZeroCopySendEvent, xZeroCopyReceiveEvent;

#else
	/* When the trace recorder code is not included just #define away the call
	to post the user event. */
	#define echoMARK_SEND_IN_TRACE_BUFFER( x )
	#define xZeroCopySendEvent 0
	#define xZeroCopyReceiveEvent 0
#endif

static void prvEchoClientTask( void *pvParameters );

/* The receive timeout is set shorter when the windows simulator is used
because simulated time is slower than real time. */
#ifdef _WINDOWS_
	const TickType_t xReceiveTimeOut = 50 / portTICK_RATE_MS;
#else
	const TickType_t xReceiveTimeOut = 500 / portTICK_PERIOD_MS;
#endif

/*-----------------------------------------------------------*/

void vTaskCreateUDPServer( uint16_t usTaskStackSize, UBaseType_t uxTaskPriority )
{
	PRINTF("creating UDP Server\r\n");
	/* Create the echo client task that does not use the zero copy interface. */
	xTaskCreate( 	prvEchoClientTask,	/* The function that implements the task. */
					"Echo0",			/* Just a text name for the task to aid debugging. */
					usTaskStackSize,	/* The stack size is defined in FreeRTOSIPConfig.h. */
					NULL,				/* The task parameter, not used in this case. */
					uxTaskPriority,		/* The priority assigned to the task is defined in FreeRTOSConfig.h. */
					NULL );				/* The task handle is not used. */

}
/*-----------------------------------------------------------*/

static void prvEchoClientTask( void *pvParameters )
{
	long lBytes;
	Socket_t xSocket;
	struct freertos_sockaddr xEchoServerAddress ,xClient;
	char cTxString[ 25 ], cRxString[ 25 ];
	uint32_t xAddressLength = sizeof( xEchoServerAddress );
	xClient.sin_addr = 0x0A0101c0;
	xClient.sin_port = FreeRTOS_htons(1234);

	uint32_t xClientLength = sizeof( xClient );
	/* Remove compiler warning about unused parameters. */
	( void ) pvParameters;

	xEchoServerAddress.sin_port = FreeRTOS_htons( 1000 );
	xEchoServerAddress.sin_addr = 0x010101c0;// 192.1.1.1

	/* Create a socket. */
	xSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );
	configASSERT( xSocket != FREERTOS_INVALID_SOCKET );

	// Bind server to address and port
	FreeRTOS_bind(xSocket, &xEchoServerAddress, xAddressLength);

	/* Set a time out so a missing reply does not cause the task to block
	indefinitely. */
	FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
	PRINTF("UDP socket created and bound to IP:%d and Port %d \r\n", xEchoServerAddress.sin_addr, xEchoServerAddress.sin_port);

	for( ;; )
	{
		/* Create the string that is sent to the echo server. */
		sprintf( cTxString, "Heart beat from MCU\r" );

		FreeRTOS_sendto( xSocket,				/* The socket being sent to. */
						( void * ) cTxString,	/* The data being sent. */
						strlen( cTxString ) + 1,/* The length of the data being sent. */
						0,						/* ulFlags with the FREERTOS_ZERO_COPY bit clear. */
						&xClient,	/* The destination address. */
						sizeof( xClientLength ) );


		memset( ( void * ) cRxString, 0x00, sizeof( cRxString ) );
		lBytes = FreeRTOS_recvfrom(	xSocket,				/* The socket being received from. */
							cRxString,				/* The buffer into which the received data will be written. */
							sizeof( cRxString ),	/* The size of the buffer provided to receive the data. */
							0,						/* ulFlags with the FREERTOS_ZERO_COPY bit clear. */
							&xClient,	/* The address from where the data was sent (the source address). */
							&xClientLength );

	   if( lBytes > 0 )
	   {
			// get the command type and ensure its for pwm adjustment. Must start with a '4'
//			char command = cRxString[0];
			PRINTF("%s \r\n",cRxString); //Just print the received string if it is not a PWM command
	   }
		/* Pause for a short while to ensure the network is not too
		congested. */
		vTaskDelay( echoLOOP_DELAY );
	}
}
/*-----------------------------------------------------------*/
