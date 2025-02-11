#include "utils.h"

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
	
	clock_t start = clock();
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
     


        
        sending=0;
	}

	 clock_t end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Message sent successfully\n");
    printf("Time taken to send the message: %f\n", time_taken);
    printf("Message size: %d\n", msg_size);
    printf("Bits per second: %f\n", msg_size * 8 / time_taken);
	printf("Sender finished\n");
	return 0;
}
