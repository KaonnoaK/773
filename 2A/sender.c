#include "utils.h"

/*
 * For a clock length of config->interval,
 * - Sends a bit 1 to the receiver by repeatedly flushing the address
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
	// Initialize config and local variables
	struct config config;
	init_config(&config, argc, argv);
	int sending = 1;

	bool sequence[8] = {1,0,1,0,1,0,1,1};
	
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

		char *msg = string_to_binary(text_buf);

		for (int i = 0; i < 8; i++) {
			send_bit(sequence[i], &config);
		}

		size_t msg_len = strlen(msg);
		for (int ind = 0; ind < msg_len; ind++) {
			if (msg[ind] == '0') {
				send_bit(false, &config);
			} else {
				send_bit(true, &config);
			}
		}
		
		
		
	printf("Sender finished\n");
	return 0;
}


