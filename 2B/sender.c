 #include "utils.h"


void send_bit(bool one, Node* head)
{

	CYCLES start_t = cc_sync();
	if (one) {

		ADDR_PTR addr = head;
		while ((get_time() - start_t) < INTERVAL) {
			pointer_chase(addr);
		}	

	} else {
	
		while (get_time() - start_t < INTERVAL) {}
	}
}

int main() {

	
	int sending = 1;

	Node *head = NULL;
	create_ll(&head,MT);

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
   clock_t start = clock();
		char *msg = string_to_binary(text_buf);

		for (int i = 0; i < 8; i++) {
			send_bit(sequence[i], head);
		}

		size_t msg_len = strlen(msg);
		for (int ind = 0; ind < msg_len; ind++) {
			if (msg[ind] == '0') {
				send_bit(false, head);
			} else {
				send_bit(true, head);
			}
		}
		
		
		
	clock_t end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Message sent successfully\n");
    printf("Time taken to send the message: %f\n", time_taken);
    printf("Message size: %d\n", msg_size);
    printf("Bits per second: %f\n", msg_size * 8 / time_taken);
	return 0;
}

