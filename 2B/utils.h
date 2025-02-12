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
#define MAX_ACCESSES 100

#ifndef UTIL_H_
#define UTIL_H_

#define CYCLES uint32_t
#define ADDR_PTR uint64_t
#define CACHE_SIZE (8*1024*1024)
#define CACHE_LINE_SIZE 64
#define HALF_CACHE_SIZE (CACHE_SIZE/2)
#define NUM_NODES (HALF_CACHE_SIZE / CACHE_LINE_SIZE )
#define NUM_SETS (NUM_NODES / 16)

#ifdef DUPLEX_MODE
#define CHANNEL_SENDING_INTERVAL        0x00010000  // Increased interval for more cache filling time
#define CHANNEL_SYNC_TIMEMASK           0x0001FFFF  // Increased time mask for lenient synchronization
#else
#define CHANNEL_SENDING_INTERVAL        0x00010000  // Increased interval for more cache filling time
#define CHANNEL_SYNC_TIMEMASK           0x0001FFFF  // Increased time mask for lenient synchronization
#endif

#define CHANNEL_SYNC_JITTER             0x0200  // Increased jitter for better synchronization tolerance

#define MAX_BUFFER_LEN	1024



struct config {
	int interval;
	uint8_t *addr;
};

typedef struct Node {
	struct Node *next ;
	uint8_t data[CACHE_LINE_SIZE - sizeof(struct Node *)];
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

void fill_llc (uint8_t *base);

void pointer_chase(Node *head);

Node *create_ll (void);

#endif

