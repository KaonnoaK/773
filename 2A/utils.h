#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MSG_FILE "msg.txt"
#define MAX_MSG_SIZE 500

#define CHANNEL_THRESHOLD 125

#ifndef UTIL_H_
#define UTIL_H_


#define CYCLES uint32_t
#define ADDR_PTR uint64_t


#define CHANNEL_SENDING_INTERVAL        0x00009FFF
#define CHANNEL_SYNC_TIMEMASK           0x0000FFFF


#define CHANNEL_SYNC_JITTER             0x0100

#define MAX_BUFFER_LEN	1024



struct config {
	int interval;
	ADDR_PTR addr;
};

CYCLES mem_access(ADDR_PTR addr);
CYCLES rdtscp(void);
CYCLES get_time();
CYCLES cc_sync();

double check_accuracy(char*, int);

void clflush(ADDR_PTR addr);

char *string_to_binary(char *s);

char *conv_char(char *data, int size, char *msg);

void init_config(struct config *config);

#endif
