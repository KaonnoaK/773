#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#define CHANNEL_THRESHOLD 140

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

bool detect_bit(struct config *config);

/*
 * Detects a bit by repeatedly measuring the access time
 * and counting the number of detected ones/zeros
 *
 * Detect a bit 1 if ...
 * Detect a bit 0 otherwise
 */
bool detect_bit(struct config *config)
{
		int hits = 0;
		int misses = 0;

	// Sync with sender
	CYCLES start_t = cc_sync();
	while ((get_time() - start_t) < config->interval) {
		CYCLES access_time = mem_access(config->addr);
        /*printf("%d\n", access_time);*/
		// Check if detected 1 or 0
		if (access_time > CHANNEL_THRESHOLD) {
			misses++;
		} else {
			hits++;
		}
	}

	return misses >= hits;
}

int main(int argc, char **argv)
{
	// Initialize config and local variables
	struct config config;
	init_config(&config, argc, argv);
	char msg_ch[MAX_BUFFER_LEN + 1];

	uint32_t bitSequence = 0;
	uint32_t sequenceMask = ((uint32_t) 1<<6) - 1;
	uint32_t expSequence = 0b101011;
	int signal =1;
	
	printf("Receiver ready \n");
	fflush(stdout);
	while (signal) {
		bool bitReceived = detect_bit(&config);

		// Detect the sequence '101011' that indicates sender is sending a message	
		bitSequence = ((uint32_t) bitSequence<<1) | bitReceived;
		if ((bitSequence & sequenceMask) == expSequence) {
			int binary_msg_len = 0;
			int strike_zeros = 0;
			for (int i = 0; i < MAX_BUFFER_LEN; i++) {
				binary_msg_len++;

				if (detect_bit(&config)) {
					msg_ch[i] = '1';
					strike_zeros = 0;
				} else {
					msg_ch[i] = '0';
					if (++strike_zeros >= 8 && i % 8 == 0) {
						break;
					}
				}
			}
			msg_ch[binary_msg_len - 8] = '\0';

			// Print out message
			int ascii_msg_len = binary_msg_len / 8;
			char msg[ascii_msg_len];
			printf("\n %s\n", conv_char(msg_ch, ascii_msg_len, msg));
	
			
		}
		
		
	}

	printf("Receiver finished\n");
	return 0;
}
