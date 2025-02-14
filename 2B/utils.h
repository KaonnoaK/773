#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MSG_FILE "msg.txt"
#define MAX_MSG_SIZE 500

#define CHANNEL_THRESHOLD 150
#define MAX_ACCESSES 5

#ifndef UTIL_H_
#define UTIL_H_

#define CYCLES uint32_t
#define ADDR_PTR uint64_t
#define CACHE_SIZE (8*1024*1024)
#define CACHE_LINE_SIZE 64
#define WAYS 16
#define SETS 8192
#define HALF_CACHE_SIZE (CACHE_SIZE/2)
#define INTERVAL 0x00F00000
#define NUM_NODES (HALF_CACHE_SIZE / CACHE_LINE_SIZE / 4)
#define NUM_SETS (NUM_NODES / 16)


#ifdef DUPLEX_MODE
#define CHANNEL_SENDING_INTERVAL        0x00020000  // Increased interval for more cache filling time
#define CHANNEL_SYNC_TIMEMASK           0x003FFFFF
#else
#define CHANNEL_SENDING_INTERVAL        0x00010000  // Increased interval for more cache filling time
#define CHANNEL_SYNC_TIMEMASK           0x001FFFFF  // Increased time mask for lenient synchronization
#endif

#define CHANNEL_SYNC_JITTER             0x0000003F  // Increased jitter for better synchronization tolerance

#define MAX_BUFFER_LEN	1024

struct config {
	int interval;
	uint64_t *addr;
};

typedef struct Node {
	struct Node *next ;
	uint64_t data;
}Node;

CYCLES mem_access(ADDR_PTR addr);
CYCLES rdtscp(void);
CYCLES get_time();
CYCLES cc_sync();

double check_accuracy(char*, int);

void clflush(ADDR_PTR addr);

char *string_to_binary(char *s);

char *conv_char(char *data, int size, char *msg);

void init_configS(struct config *config, int argc, char **argv);

void init_configR(struct config *config, int argc, char **argv);

void fill_llc (uint64_t *base);

void pointer_chase(Node *head);

void create_ll (Node**);

#endif

