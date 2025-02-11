#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MSG_FILE "msg.txt"
#define MAX_MSG_SIZE 500

#ifndef UTIL_H_
#define UTIL_H_

#define CYCLES uint32_t
#define ADDR_PTR uint64_t

#ifdef DUPLEX_MODE
#define CHANNEL_SENDING_INTERVAL        0x00009FFFF
#define CHANNEL_SYNC_TIMEMASK           0x0000FFFFF
#else
#define CHANNEL_SENDING_INTERVAL        0x00009FFF
#define CHANNEL_SYNC_TIMEMASK           0x0000FFFF
#endif

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


void clflush(ADDR_PTR addr);

char *string_to_binary(char *s);

char *conv_char(char *data, int size, char *msg);

void init_config(struct config *config, int argc, char **argv);




#endif


/*
 * Loads from virtual address addr and measure the access time
 */
CYCLES mem_access(ADDR_PTR addr)
{
    CYCLES cycles;

    asm volatile("mov %1, %%r8\n\t"
            "lfence\n\t"
            "rdtsc\n\t"
            "mov %%eax, %%edi\n\t"
            "mov (%%r8), %%r8\n\t"
            /*"lfence\n\t"*/
            "rdtscp\n\t"
            "sub %%edi, %%eax\n\t"
    : "=a"(cycles) /*output*/
    : "r"(addr)
    : "r8", "edi");

    return cycles;
}


/*
 * Flushes the cache block accessed by a virtual address out of the cache
 */
extern inline __attribute__((always_inline))
void clflush(ADDR_PTR addr)
{
    asm volatile ("clflushopt (%0)"::"r"(addr));
}


/* 
 * Returns Time Stamp Counter 
 */
extern inline __attribute__((always_inline))
CYCLES rdtscp(void) {
	CYCLES cycles;
	asm volatile ("rdtscp"
	: /* outputs */ "=a" (cycles));

	return cycles;
}

/* 
 * Gets the value Time Stamp Counter 
 */
inline CYCLES get_time() {
    return rdtscp();
}

/* Synchronizes at the overflow of a counter
 *
 * Counter is created by masking the lower bits of the Time Stamp Counter
 * Sync done by spinning until the counter is less than CHANNEL_SYNC_JITTER
 */
extern inline __attribute__((always_inline))
CYCLES cc_sync() {
    while((get_time() & CHANNEL_SYNC_TIMEMASK) > CHANNEL_SYNC_JITTER) {}
    return get_time();
}

/*
 * Convert a given ASCII string to a binary string.
 * From:
 * https://stackoverflow.com/questions/41384262/convert-string-to-binary-in-c
 */
char *string_to_binary(char *s)
{
    if (s == NULL) return 0; /* no input string */

    size_t len = strlen(s) - 1;

    // Each char is one byte (8 bits) and + 1 at the end for null terminator
    char *binary = malloc(len * 8 + 1);
    binary[0] = '\0';

    for (size_t i = 0; i < len; ++i) {
        char ch = s[i];
        for (int j = 7; j >= 0; --j) {
            if (ch & (1 << j)) {
                strcat(binary, "1");
            } else {
                strcat(binary, "0");
            }
        }
    }

    return binary;
}

/*
 * Convert 8 bit data stream into character and return
 */
char *conv_char(char *data, int size, char *msg)
{
    for (int i = 0; i < size; i++) {
        char tmp[8];
        int k = 0;

        for (int j = i * 8; j < ((i + 1) * 8); j++) {
            tmp[k++] = data[j];
        }

        char tm = strtol(tmp, 0, 2);
        msg[i] = tm;
    }

    msg[size] = '\0';
    return msg;
}

/*
 * Prints help menu
 */
void print_help() {

	printf("DeadDrop Covert Channel Sender/Reseiver Flags:\n"
		"\t-i,\tTime interval for sending a single bit\n");

}

/*
 * Returns the 10 bits cache set index of a given address.
 */
uint64_t get_cache_set_index(ADDR_PTR phys_addr)
{
    uint64_t mask = ((uint64_t) 1 << 16) - 1;
    return (phys_addr & mask) >> 6;
}

/*
 * Parses the arguments and flags of the program and initializes the struct config
 * with those parameters (or the default ones if no custom flags are given).
 */
void init_config(struct config *config, int argc, char **argv)
{
		int *data = (int*) malloc(64*64*64);

		config->interval = CHANNEL_SENDING_INTERVAL;

		int *addr = data;
        while(get_cache_set_index((ADDR_PTR) addr)) {
            addr += 64;
        }
	
		config->addr = (ADDR_PTR) addr;

		/*
	// Parse the command line flags
	int option;
	while ((option = getopt(argc, argv, "i:o:f:")) != -1) {
		switch (option) {
			case 'i':
				echit(1);
			default:
				print_help();
				exit(1);
		}
	}
	
	*/
}

void send_bit(bool one, struct config *config);

/*
 * For a clock length of config->interval,
 * - Sends a bit 1 to the receiver by ...
 * - Sends a bit 0 by doing nothing
 */
void send_bit(bool one, struct config *config)
{
	// Synchronize with receiver
	CYCLES start_t = cc_sync();
	if (one) {
		// Repeatedly flush addr
		ADDR_PTR addr = config->addr;
		while ((get_time() - start_t) < config->interval) {
				clflush(addr);
		}	

	} else {
		// Do Nothing
		while (get_time() - start_t < config->interval) {}
	}
}

int main(int argc, char **argv) {
	
	
    FILE *fp = fopen(MSG_FILE, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        return 1;
    }

    char text_buf[MAX_MSG_SIZE];
    int msg_size = 0;
    char c;
    while((c = fgetc(fp)) != EOF){
        text_buf[msg_size++] = c;
    }
    fclose(fp);
	
	
	struct config config;
	init_config(&config, argc, argv);
	int sending = 1;

        clock_t start_t, end_t;

	bool sequence[8] = {1,0,1,0,1,0,1,1};

	printf("\n Starting \n");
	while (sending) {
        /*cc_sync();*/
        /*clflush(config.addr);*/
        /*continue;*/

		

		// Convert that message to binary
		char *msg = string_to_binary(text_buf);

		// Send a '10101011' bit sequence tell the receiver
		// a message is going to be sent
		for (int i = 0; i < 8; i++) {
			send_bit(sequence[i], &config);
		}

		// Send the message bit by bit
		size_t msg_len = strlen(msg);
        start_t = clock();
		for (int ind = 0; ind < msg_len; ind++) {
			if (msg[ind] == '0') {
				send_bit(false, &config);
			} else {
				send_bit(true, &config);
			}
		}
        end_t = clock();

        printf("Bitrate: %.2f Bytes/second\n", ((double) msg_len) / ((double) (end_t - start_t) / CLOCKS_PER_SEC));
        
        sending=0;
	}

	printf("Sender finished\n");
	return 0;
}
