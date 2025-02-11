#include "utils.h"

bool detect_bit(struct config *config);

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
	int ascii_msg_len;
	
	printf("Receiver ready \n");
	
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
			ascii_msg_len = binary_msg_len / 8;
			char msg[ascii_msg_len];
			printf("\n %s\n", conv_char(msg_ch, ascii_msg_len, msg));
	
			
		}
		
	}
	printf("Receiver finished\n");
	printf("Accuracy (%%): %f\n", check_accuracy(msg_ch, ascii_msg_len)*100);
	
	return 0;
}
