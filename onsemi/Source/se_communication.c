/*
 * se_communication.c
 *
 *  Created on: Mar 5, 2024
 *      Author: Stanley.Ezeh
 */

#include "se_communication.h"
#include "se_udp_communication.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"


#define SE_DEBUG 1

uint8_t g_index; // used for storing the last table index
size_t g_table_size; // store the table size
extern uint8_t t_buff[];

QueueHandle_t tx_queue, rx_queue;
TaskHandle_t comm_task_handle;
uint8_t active_connection =0;
SE_UDP_NET_DRV_V4_t udp_drv_obj;
extern const mem_record_t MEM_TABLE;

//const char *SE_TABLES[MAX_NUMBER_TABLES];
#ifdef se_debug
static void print_buffer(uint8_t *buffer);
#endif // se_debug
static void comm_receive_task(void *arg);
static void comm_send_task(void *arg);

error_t comm_task_init(void) {

	// initialize a communication queue
	rx_queue = xQueueCreate( COMM_QUEUE_SIZE, sizeof(command_t ) );
	if( rx_queue == NULL)
	{
	    // Queue was not created and must not be used.
		PRINTF("Queue was not created \n");
		return FAIL;
	}


	if(xTaskCreate(comm_receive_task, "receive_task", 1024*2, NULL, tskIDLE_PRIORITY+3, NULL) !=pdPASS){
		PRINTF("Failed to create comm receive task \n");
		return FAIL;
	}

	if(xTaskCreate(comm_send_task, "send_task", 1024, NULL, tskIDLE_PRIORITY+2, NULL) !=pdPASS){
		PRINTF("Failed to create comm send task \n");
		return FAIL;
	}

	PRINTF("Receive and Send task created successfully \n");

	return SUCCESS;
}



static void comm_receive_task(void *arg){

	// Initialize UDP stack
	DelayMs(50);
	SE_Net_Addr_V4_t ipconf = {
			.dn = "UDP Eth0",
			.sender_addr = {192,1,1,5},  // default to broadcast
			.sender_port = 12345,
			.port = SE_UDP_PORT,
			.ipv4_addr = {192,1,1,2}
	};


	udp_drv_obj.ipconfig = ipconf;
	se_udp_comm_init(&udp_drv_obj);
	ipconfig(&udp_drv_obj);

	uint8_t msg_buff[SE_MAX_BUFF_SIZE];
	uint32_t recv_sz = 0;
	PRINTF("RECEIVE Task started \n");
	DelayMs(50);

	for(;;){

		recv_sz = udp_drv_obj.Recv(msg_buff, SE_MAX_BUFF_SIZE, &udp_drv_obj.ipconfig);

		if(recv_sz >0){
			// theres a pending message in the buffer
			PRINTF("Received %d bytes. sending to command parser for processing \n", recv_sz);

			//TODO check if it is a valid protocol command. if not drop the packet
			// read the message header


			// convert buffer to command type. I know this means doing it twice but it simplifies things
			command_t cmd;
			if(rebuild_cmd_struct(msg_buff, &cmd, recv_sz) !=SUCCESS){
				PRINTF("Failed to rebuild data from received byte array. Dropping ... \n");
			}
			else{
				PRINTF("%s :: Sending command %d to the receive queue\n", __func__, cmd.header.transaction_id);
				send_command_to_queue(&cmd);
			}

		}

	}
	vTaskDelay(pdMS_TO_TICKS(2000));
}


uint8_t read_table_record(void *address, uint8_t *buffer, uint8_t size) {

	// perform memory copy of the data byte by byte

	// return how many bytes copied

	uint8_t * src = (uint8_t *)address;
	uint8_t * dst = (uint8_t *)buffer;

	uint8_t count=0;

	for(count=0; count<size; count++){
		*dst = *src;
		dst++;
		src++;
	}

	return count;

}

error_t parse_command(command_t *in_cmd, const mem_record_t *mem_table) {
	error_t err = SUCCESS;
	if (in_cmd == NULL)
		return FAIL;

	uint8_t data_size = 0;
	TCommand cmd = in_cmd->header.command_type;
	data_size = in_cmd->txrx_size;

	SE_Net_Addr_V4_t destination_addr= udp_drv_obj.ipconfig;


	if (cmd == load) {
		if (load_table(mem_table) != SUCCESS) {
			err = FAIL;
		}
		return err;
	}


	// check that all the fields are correctly set
	uint8_t found = 0;
	for (int i = 0; i < g_table_size; i++) {
		const mem_record_t *record = &mem_table[i];
		if (mem_table[i].address == in_cmd->address) {
			found = 1;
			switch (cmd) {
			case read: {

				if (record->attribute == RO || record->attribute == RW) {
					if (read_table_record(in_cmd->address, in_cmd->data, data_size) != data_size) {
						err = FAIL;
						// incomplete read
					}else{
						err = SUCCESS;
						// memory read correctly
						// send buffer to queue for serialing
						uint8_t buff_size = get_required_buffsize_from_cmdsize(*in_cmd);
						uint8_t buffer[MAX_TXRX_SIZE];
						// initialize the memory
						memset(buffer,0, MAX_TXRX_SIZE);
						serialize_cmd_struct(in_cmd, buffer);
						udp_comm_send(&destination_addr, buffer, buff_size);
//						udp_drv_obj.Send(&udp_drv_obj.ipconfig, buffer, buff_size);
					}
				}
				else{
					err=FAIL;
					// memory cannot be read-> permission error
					PRINTF("Cannot read tag: %s due to permission error \n", record->name);
				}
				break;
			}
			case write: {
				// verify that it is writeable
				if (record->attribute == RW || record->attribute == WO) {
					if (write_table_record(in_cmd->address, (void *)in_cmd->data,data_size) !=data_size){
						err=FAIL;
						// incomplete write
						PRINTF("Inomplete write to address 0x%p name: %s\n", in_cmd->address, record->name);
					}else{
						err=SUCCESS;
						PRINTF("WRITE complete to address 0x%pname: %s\n", in_cmd->address, record->name);
					}
				}else{
					err=FAIL;
					// memory cannot be written-> permission error
					PRINTF("Write permission error: name: %s\n", record->name);
				}

				break;
			}
			default:
				PRINTF("Invalid command received \n");
				break;
			}

		}


	}

	if(found==0) PRINTF("The requested memory is not found \n");
	return err;
}

uint8_t write_table_record(void *destination, void *source, uint8_t size) {

//	uint8_t * src = (uint8_t *)source;
//	uint8_t * dst = (uint8_t *)destination;
//
//	uint8_t count=0;
	PRINTF("Writing %f to memory", *(float *)source);
	memcpy(destination, source, size);

//	for(count=0; count<size; count++){
//		*dst = *src;
//		dst++;
//		src++;
//	}

	return size;
}


error_t load_table(const mem_record_t * table){

	error_t err = SUCCESS;
	uint8_t buff_size =0;
	uint8_t buffer[256];

	for(int i=0; i<g_table_size; i++){

		memset(buffer,0, 256);
		buff_size = get_mem_record_size(&table[i]);
		serialize_mem_record(&table[i], buffer);

		// send the data to the transmission queue;
//		if(xQueueSend(tx_queue, &msg, pdMS_TO_TICKS(2)) !=pdPASS){
//			PRINTF("Failed to send message to TX queue \n");
//			return FAIL;
//		}
		udp_comm_send(&udp_drv_obj.ipconfig, buffer, buff_size);
//		udp_drv_obj.Send(&udp_drv_obj.ipconfig, buffer, buff_size );
	}

	return err;
}

#ifdef se_debug
static void print_buffer(uint8_t *buffer){
	mem_record_t mem_record ={};
	mem_record.address = (void *)(uint32_t *)(&buffer[1]);
	mem_record.size = buffer[5];
	mem_record.data_type = (dt_map)buffer[6];
	mem_record.attribute = buffer[7];


//	memcpy((void *)&mem_record.name, &buffer[7], 4);


	PRINTF("Address: 0x%p | size: %d | data_type: %s |attribute : %s |name: ",
		(uint32_t*) mem_record.address, mem_record.size,
		mem_record.data_type == sU32 ? "sU32" :
		(mem_record.data_type == sU8) ? "sU8" : "sU16",
		mem_record.attribute == RO ? "RO" :
		(mem_record.attribute == RW) ? "RW" : "WO");

	for(int i=8; buffer[i]!='\0'; i++){
		if(buffer[i] == STOP_SYMBOL) break;
		PRINTF("%c", buffer[i]);
	}
	PRINTF("\n");
}
#endif //se_debug

static void comm_send_task(void *arg){
	//  check message queue to see if there is data to be sent
	command_t msg={};
	PRINTF("SEND TASK STARTED \n");
	for(;;){

		while (uxQueueMessagesWaiting(rx_queue) > 0) {
			// fetch
			if (xQueueReceive(rx_queue, &msg, 50) != pdPASS) {
				PRINTF("Failed to fetch from queue\n");
			} else {
				PRINTF("%s :: Processing command id: %d \n",__func__,  msg.header.transaction_id);
				parse_command(&msg, &MemTable);
			}

		}
	}
	vTaskDelay(pdMS_TO_TICKS(2000));
}

uint8_t get_mem_record_size(const mem_record_t *mem_record){
	uint8_t cnt;
	for(cnt=0; mem_record->name[cnt] !='\0'; cnt++);

	return sizeof(mem_record->address) + sizeof(mem_record->data_type) + \
			sizeof(mem_record->attribute) + sizeof(mem_record->size) +cnt +2;
}

void serialize_mem_record(const mem_record_t *mem_record, uint8_t * buffer){

	uint8_t cnt;
	for(cnt=0; mem_record->name[cnt] !='\0'; cnt++);

	uint8_t size = get_mem_record_size(mem_record) + 2 + cnt;

	buffer[0] = START_SYMBOL_MEM_RECORD;
	int i=1;
	while(i<size-1){
		switch (i) {
			case 1: // address 4 bytes
				memcpy((void *)&buffer[i], (void*)(&mem_record->address),  4);
				i=4;
				break;
			case 5: // size 1 byte
				memcpy((void *)&buffer[i], (void*)(&mem_record->size),  1);
				break;
			case 6:  // data type 1 byte
				memcpy((void *)&buffer[i], (void*)(&mem_record->data_type),  1);
				break;
			case 7: //attribute 1 byte
				memcpy( (void *)&buffer[i], (void*)(&mem_record->attribute), 1);
				break;
			case 8: //name
//				sprintf((char *)&buffer[i], me)
				memcpy((void *)&buffer[i], (void*)(mem_record->name),  cnt);
				i+=cnt;
				goto cont; // jump to closing symbol
				break;
			default:
				break;
		}

		i++;
	}
	cont:
		buffer[i] = STOP_SYMBOL;

}

// copy, byte-by-byte the entire struct and store in the buffer
void serialize_cmd_struct(command_t *in_cmd, uint8_t* buffer){
	// add start and stop bytes to mark boundary
	uint8_t * cp_buff = buffer;
	*cp_buff = START_SYMBOL;  // start of byte array
	cp_buff++;

	// copy the header
	*(uint32_t*)cp_buff= (uint32_t)in_cmd->header.transaction_id;
	cp_buff+=4;
	*cp_buff = in_cmd->header.command_type;
	cp_buff++;

	// copy the address
	*(uint32_t*)cp_buff = (uint32_t)in_cmd->address;  // we know that the void pointer is 4 bytes
	cp_buff +=4;

	// copy the transmit/receive data size
	*cp_buff = in_cmd->txrx_size;
	cp_buff++;

	// copy the data
	for(int i=0; i<in_cmd->txrx_size; i++){
		*cp_buff = in_cmd->data[i];
		cp_buff++;
	}

	*cp_buff = STOP_SYMBOL; // end of bytearray
}

// convert buffer to cmd struct
error_t rebuild_cmd_struct(uint8_t *buffer, command_t *out_cmd, uint8_t buffer_size){

	error_t err = SUCCESS;
	// we must use correct indices this time
	/*
	 * Buffer structure
	 * 0 - sentinel \0  (1B)
	 * 1 - command_type (2B)
	 * 3 - command_transaction_id (4B)
	 * 7 - memory_address (4B)
	 * 11 - transmit/receive data size (1B)
	 * 12 - begining of data (txrx_size B)
	 * 12+txrx_size+1 - sentinel
	 * */
	uint8_t  * tmp = buffer;
	if(tmp[0] != START_SYMBOL) {
		// check if the first value in the buffer is a sentinel
		PRINTF("Invalid buffer \n");
		err = FAIL;
	}else
	{

		uint8_t i=1;
		while(i<buffer_size-1){
			switch (i) {
				case 1: // transaction id
					memcpy((void*)(&out_cmd->header.transaction_id), (void *)&buffer[i], 4);
					i=4;
					break;
				case 5: // command type
					memcpy((void*)(&out_cmd->header.command_type), (void *)&buffer[i], 1);
					break;
				case 6:  // memory address
					memcpy((void*)(&out_cmd->address), (void *)&buffer[i], 4);
					i=9;
					break;
				case 10: //txrx_size
					memcpy((void*)(&out_cmd->txrx_size), &buffer[i], 1);
					break;
				case 11:
					memcpy((void*)(&out_cmd->data), &buffer[i], buffer[10]);
					i+=buffer[10];
					break;
				default:
					break;
			}

			i++;
		}
	}

	return err;
}


uint8_t get_required_buffsize_from_cmdsize(command_t in_cmd){
	return 2 + sizeof(in_cmd.address) + in_cmd.txrx_size + sizeof(in_cmd.header.command_type) +sizeof(in_cmd.header.transaction_id) + sizeof(in_cmd.txrx_size);
}


void send_command_to_queue(const command_t *cmd) {
//    command_t copyCmd; // Create a local copy of the command
//
//    // Copy the contents of cmd to copyCmd
//    memcpy(&copyCmd, cmd, sizeof(command_t));

    // Now send copyCmd to the queue
    if (xQueueSend(rx_queue, cmd, portMAX_DELAY) != pdPASS) {
        // Handle queue full scenario here if needed
    	PRINTF("Failed to send command to queue\n");
    }
}
