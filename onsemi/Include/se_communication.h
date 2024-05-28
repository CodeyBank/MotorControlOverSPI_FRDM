/*
 * se_communication.h
 *
 *  Created on: Mar 5, 2024
 *      Author: Stanley.Ezeh
 */

#ifndef INCLUDE_SE_COMMUNICATION_H_
#define INCLUDE_SE_COMMUNICATION_H_

#include "FreeRTOS.h"
#include "onsemi_hardware.h"

#define tU8 uint8_t
#define tU16 uint16_t
#define tU32 uint32_t
#define tU32ptr tU32 *
#define tCCh const char *
#define tUCh const unsigned char *
#define tCh char*
#define tVoidPtr void *

#define MAX_TXRX_SIZE 50   // max of 64bits
#define MAX_NUMBER_TABLES 8
#define SE_MAX_BUFF_SIZE 50
#define START_SYMBOL '+'
#define START_SYMBOL_MEM_RECORD '*'
#define STOP_SYMBOL '-'
#define COMM_QUEUE_SIZE 10

typedef enum tcommand_types {
	read = 0xA1, write = 0xA2, load = 0xB1
} TCommand;

typedef struct commandheader {
	uint32_t transaction_id;
	TCommand command_type;
} commandheader_t;

typedef enum data_types {
	sU8 = 0x51, sU16, sU32, sFLT, sDFLT
} dt_map;

typedef enum attribute {
	RO = 0xF0, RW, WO
} mem_record_attr;

typedef struct command {

	commandheader_t header;
	void * address; // read/write index in the table to avoid use of hashtables
	uint8_t txrx_size;  // tx/rx size
	uint8_t data[MAX_TXRX_SIZE];  // buffer

} command_t;

typedef struct Message_S{

	uint8_t size;
	uint8_t * buffer;
}SE_Message_t;

/**
 * @brief command_parser decodes the command and decides which command
 * 		  is called before dispatching to the command handler. Basically
 * 		  performs data deserialization
 * @params user_command
 *
 * return structure containing the type of command being serviced and the command_id
 **/

typedef struct memory_table_record {

	void * address;
	uint8_t size;
	dt_map data_type;
	mem_record_attr attribute;
	char *name;
} mem_record_t;

extern const char *SE_TABLES[];
extern uint8_t g_index;

static inline error_t se_add_table(const char *table_name) {
	error_t err = SUCCESS;
	if (g_index > MAX_NUMBER_TABLES) {
#ifdef SE_DEBUG
		PRINTF("Error trying to add to full table\n");
#endif //SE_DEBUG
		err = FAIL;
	} else {
		SE_TABLES[g_index] = table_name;
		g_index++;
#ifdef SE_DEBUG
		PRINTF("Table added successfully. Table Size = %d\n", g_index);
#endif //SE_DEBUG
	}
	return err;
}
// Exported Macros

#define RECORD_TYPE mem_record_t
#define MEM_TABLE MemTable

#define MEMORY_TABLE_DECL_START(name) \
	const RECORD_TYPE name[] = {

#define MEMORY_TABLE_DECL_CLOSE()	\
	};

#define CONVERT_TO_STR(str)  (tCh)(#str)
#define CONVERT_TO_U8(val) (tU8)val
#define CONVERT_TO_U16(val) (tU16)val
#define CONVERT_TO_U32_PTR(val) (tU32ptr)val
#define CONVERT_TO_U8(val) (tU8)val
#define CAST_TO_MEM_ATTR(val) (mem_record_attr)val
#define CAST_TO_DT_TYPE(val)  (dt_map)val
#define GET_ADDR(variable) (void *)(&variable)
#define VAR_SIZE(variable) sizeof(variable)

#define SE_MEM_REC_CREATE(_name, _data_type, _attribute) \
		{ .name=CONVERT_TO_STR(_name), \
		  .address = GET_ADDR(_name), \
		  .size = VAR_SIZE(_name), \
		  .data_type=CAST_TO_DT_TYPE(_data_type), \
		  .attribute=CAST_TO_MEM_ATTR(_attribute) \
		},



error_t comm_task_init(void);

/**
 * @brief read a value from the desired table
 *
 * @params
 *
 * @return
 * */
uint8_t read_table_record(void *address, uint8_t *buffer, uint8_t size) ;

/**
 * @brief
 *
 * @params
 *
 * @return
 * */
uint8_t write_table_record(void *destination, void *source, uint8_t size);

/**
 * @brief
 *
 * @params
 *
 * @return
 * */
error_t load_table(const mem_record_t * table);

/**
 * @brief
 *
 * @params
 *
 * @return
 * */
error_t parse_command(command_t *in_cmd, const mem_record_t *mem_table);

/**
 * @brief
 *
 * @params
 *
 * @return
 * */
void serialize_cmd_struct(command_t *in_cmd, uint8_t* buffer);

/**
 * @brief
 *
 * @params
 *
 * @return
 * */
uint8_t get_required_buffsize_from_cmdsize(command_t in_cmd);

/**
 * @brief
 *
 * @params
 *
 * @return
 * */
error_t rebuild_cmd_struct(uint8_t *buffer, command_t *out_cmd, uint8_t buffer_size);

/**
 * @brief
 *
 * @params
 *
 * @return
 * */
void serialize_mem_record(const mem_record_t *mem_record, uint8_t * buffer);


/**
 * @brief
 *
 * @params
 *
 * @return
 * */
uint8_t get_mem_record_size(const mem_record_t *mem_record);

/**
 * @brief
 *
 * @params
 *
 * @return
 * */
void send_command_to_queue(const command_t *cmd);
#endif /* INCLUDE_SE_COMMUNICATION_H_ */
