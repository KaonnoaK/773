#include "utils.h"

int CACHE_MISS_LATENCY = 120;

bool detect_bit(struct config *config)
{
	int misses = 0;
	int hits = 0;

	CYCLES start_t = cc_sync();
	while ((get_time() - start_t) < config->interval) {
		CYCLES access_time = mem_access(config->addr); 
		if (access_time > CACHE_MISS_LATENCY) {
			misses++;
		} else {
			hits++;
		}
	}

	return misses > hits;
}

int main( )
{

	struct config config;
	init_config(&config);
	char msg_ch[MAX_BUFFER_LEN + 1];

	uint32_t bitSequence = 0;
	uint32_t sequenceMask = ((uint32_t) 1<<6) - 1;
	uint32_t expSequence = 0b101011;
	
	printf("Listening...\n");
	while (1) {
		bool bitReceived = detect_bit(&config);

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
					++strike_zeros;
					if (strike_zeros >= 8 && i%8 ==0) {
						break;
					}
					
				}
				
			}
			
					
			msg_ch[binary_msg_len - 8] = '\0';

			int ascii_msg_len = binary_msg_len / 8;
			char msg[ascii_msg_len];
			//printf("\n%s\n", conv_char(msg_ch, ascii_msg_len, msg));
	
			
		}
	}

	 printf("\n\n\n Accuracy (%%): %f\n", check_accuracy(conv(msg_ch,ascii_msg_len,msg), ascii_msg_len)*100);
	return 0;
}

