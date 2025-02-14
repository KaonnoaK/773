#include "utils.h"

bool detect_bit(Node *addr);

 int main(int argc, char **argv)
{
	Node *head = NULL;
	create_ll(&head,FILL);


	char msg_ch[MAX_BUFFER_LEN + 1];
	bzero(msg_ch,MAX_BUFFER_LEN + 1);

	uint32_t bitSequence = 0;
	uint32_t sequenceMask = ((uint32_t) 1<<6) - 1;
	uint32_t expSequence = 0b101011;
	int cnt=0;
	printf("Listening...\n");

	while (1) {
		
		bool bitReceived = detect_bit(head);

		// Detect the sequence '101011' that indicates sender is sending a message	
		bitSequence = ((uint32_t) bitSequence<<1) | bitReceived;
		
		if ((bitSequence & sequenceMask) == expSequence) {
		//if(1){
			int binary_msg_len = 0;
			int strike_zeros = 0;
			for (int i = 0; i < MAX_BUFFER_LEN; i++) {
				binary_msg_len++;

				if (detect_bit(head)) {
				
					msg_ch[i] = '1';
					//printf("\n %c \n",msg_ch[i]);
					cnt++;
		//printf("\n \t %d \n",cnt);
					strike_zeros = 0;
				} else {
					msg_ch[i] = '0';
					//printf("\n %c \n",msg_ch[i]);
					cnt++;
		//rintf("\n \t %d \n",cnt);
					if (++strike_zeros >= 8 && i % 8 == 0) {
						break;
					}
				}
			}
			msg_ch[binary_msg_len - 8] = '\0';

			// Print out message
			int ascii_msg_len = binary_msg_len / 8;
			char msg[ascii_msg_len];
			//printf(" %s\n", conv_char(msg_ch, ascii_msg_len, msg));
			
			//printf("\n\n\n Accuracy (%%): %f\n", check_accuracy(msg_ch, ascii_msg_len)*100);
	
			
		}
	}

	printf("Receiver finished\n");
	return 0;
}
