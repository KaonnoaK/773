 #include "utils.h"

void send_bit(bool one, Node* head)
{
	// Synchronize with receiver
	
	//printf("\n\n inside send bit \n\n");
	CYCLES start_t = cc_sync();
	if (one) {

		while ((get_time() - start_t) < INTERVAL) {
				pointer_chase(head);
		}	

	} else {
		
		while (get_time() - start_t < INTERVAL) {}
	}
	//printf("\n exiting send bit \n\n");
}

int main(int argc, char **argv) {

		
	// Initialize config and local variables
	
	int sending = 1;
	
	Node *head = NULL;
	create_ll(&head,MT);

    clock_t start_t, end_t;

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
int cnt=0;
char *msg = string_to_binary(text_buf);

	while (sending) {

	//printf("\n\n inside sending looop \n\n");
		// Convert that message to binary
		
		//printf("\n %s \n",msg);
		//printf("\n\n\n %s ",text_buf);

		// Send a '10101011' bit sequence tell the receiver
		// a message is going to be sent
		for (int i = 0; i < 8; i++) {
			send_bit(sequence[i], head);
		}

		// Send the message bit by bit
		size_t msg_len = strlen(msg);
        start_t = clock();
		for (int ind = 0; ind < msg_len; ind++) {
		printf("\n %c \n",msg[ind]);
		cnt++;
		printf("\n \t %d \n",cnt);
			if (msg[ind] == '0') {
				send_bit(false, head);
			} else {
				send_bit(true, head);
			}
		}
        end_t = clock();

        printf("Bitrate: %.2f Bytes/second\n", ((double) msg_len) / ((double) (end_t - start_t) / CLOCKS_PER_SEC));
        
        sending=0;
	}

	printf("Sender finished\n");
	return 0;
}
