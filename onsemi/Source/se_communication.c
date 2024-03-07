/*
 * se_communication.c
 *
 *  Created on: Mar 5, 2024
 *      Author: Stanley.Ezeh
 */

#include "se_communication.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#define SE_DEBUG

uint8_t g_index; // used for storing the last table index
size_t g_table_size; // store the table size


QueueHandle_t tx_queue, rx_queue;


//const char *SE_TABLES[MAX_NUMBER_TABLES];

static void print_buffer(uint8_t *buffer, uint8_t size);

error_t memory_table_init(void) {

	// initialize a communication queue
	tx_queue = xQueueCreate( COMM_QUEUE_SIZE, sizeof( uint8_t * ) );
	rx_queue = xQueueCreate( COMM_QUEUE_SIZE, sizeof( uint8_t * ) );
	if( tx_queue == 0 || rx_queue == NULL)
	{
	      // Queue was not created and must not be used.
		PRINTF("Queue was not created \n");
		return FAIL;
	}


	return SUCCESS;
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

error_t parse_command(command_t *in_cmd, mem_record_t *mem_table) {
	error_t err = SUCCESS;
	if (in_cmd == NULL)
		return FAIL;

	uint8_t data_size = 0;

	// check that all the fields are correctly set

	for (int i = 0; i < g_table_size; i++) {
		if (mem_table[i].address == in_cmd->address) {

			TCommand cmd = in_cmd->header.command_type;
			data_size = in_cmd->txrx_size;
			mem_record_t *record = &mem_table[i];

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
					}
				}
				else{
					err=FAIL;
					// memory cannot be read-> permission error
				}
				break;
			}
			case write: {
				// verify that it is writeable
				if (record->attribute == RW || record->attribute == WO) {
					if (write_table_record(in_cmd->address, (void *)in_cmd->data,data_size) !=data_size){
						err=FAIL;
						// incomplete write
					}else{
						err=SUCCESS;
						// mem read correctly

						// send buffer to queue for serialing
					}
				}else{
					err=FAIL;
					// memory cannot be written-> permission error
				}

				break;
			}
			case load:{
				if(load_table(mem_table) !=SUCCESS){
					err = FAIL;
					// faile to load table
				}else{
					err = SUCCESS;
					// table loaded successfully
				}
				break;
			}
			default:
				break;
			}

		}


	}

	// send the result to the queue -> create a command data structure and fill it in/ or use bytes
	return err;
}

uint8_t write_table_record(void *destination, void *source, uint8_t size) {

	uint8_t * src = (uint8_t *)source;
	uint8_t * dst = (uint8_t *)destination;

	uint8_t count=0;

	for(count=0; count<size; count++){
		*dst = *src;
		dst++;
		src++;
	}

	return count;
}

error_t load_table(const mem_record_t * table){
	// for each record, call the serialization function.
	// Sentinel might be needed in the serialization function
	error_t err = SUCCESS;

	// start by checking the size of the record. this should return
	// the required size of the buffer to be used. To avoid dynamic memory allocation
	// for data, a fixed sized buffer with max size is used
	// first attempt using DMA.

	uint8_t buff_size =0;
	uint8_t *buffer = NULL;
	for(int i=0; i<g_table_size; i++){
		// get the size of the structure
		buff_size = get_mem_record_size(&table[i]);

		// create memory for the buffer
		if((buffer = (uint8_t *)malloc(buff_size)) == NULL){
			PRINTF("Failed to allocate memory for buffer \n");
			return FAIL;
		}

		// serialize the data and store in the buffer
		serialize_mem_record(&table[i], buffer);

		// send the data to the transmission queue;
		if(xQueueSend(tx_queue, &buffer, pdMS_TO_TICKS(2)) !=pdPASS){
			PRINTF("Failed to send message to TX queue \n");
			return FAIL;
		}
	}


	// CHECK

#if 0
	uint8_t * recvb =NULL;
	while(uxQueueMessagesWaiting(tx_queue)>0){
		// fetch
		if(xQueueReceive(tx_queue, &recvb, 2)!=pdPASS){
			PRINTF("Failed to fetch from queue\n");
		}
		print_buffer(recvb, buff_size);
	}

#endif //0 CHECK
	return err;

}

static void print_buffer(uint8_t *buffer, uint8_t size){
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
		PRINTF("%c", buffer[i]);
		if(buffer[i] == STOP_SYMBOL) break;
	}
	PRINTF("\n");
}



uint8_t get_mem_record_size(const mem_record_t *mem_record){
	uint8_t cnt;
	for(cnt=0; mem_record->name[cnt] !='\0'; cnt++);

	return sizeof(mem_record->address) + sizeof(mem_record->data_type) + \
			sizeof(mem_record->attribute) + sizeof(mem_record->size) +cnt;
}

void serialize_mem_record(const mem_record_t *mem_record, uint8_t * buffer){

	uint8_t cnt;
	for(cnt=0; mem_record->name[cnt] !='\0'; cnt++);

	uint8_t size = get_mem_record_size(mem_record) + 2 + cnt;

	buffer[0] = START_SYMBOL;
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
void serialize_data(command_t *in_cmd, uint8_t* buffer){
	// add start and stop bytes to mark boundary
	uint8_t * cp_buff = buffer;
	*cp_buff = '\0';  // start of byte array
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

	*cp_buff = '\0'; // end of bytearray
}

// convert buffer to cmd struct
error_t deserialize_data(uint8_t *buffer, command_t *out_cmd, uint8_t buffer_size){

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
	if(tmp[0] != '\0') {
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
					memcpy((void*)(&out_cmd->data), &buffer[i], buffer[11]);
					i+=out_cmd->txrx_size;
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
