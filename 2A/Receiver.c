 #include "utils.h"

bool detect_bit(struct config *config);

int main(int argc, char **argv) {
    // Initialize config and local variables
    struct config config;
    init_config(&config, argc, argv);
    char msg_ch[MAX_BUFFER_LEN + 1];  // Message buffer for storing bits

    uint32_t bitSequence = 0;
    uint32_t sequenceMask = ((uint32_t) 1 << 6) - 1;  // Mask to check for 6 bits
    uint32_t expSequence = 0b101011;  // Expected start sequence: 101011

    printf("Listening...\n");
    fflush(stdout);

    // Flag to prevent repeated start sequence detection
    bool startSequenceDetected = false;

    while (1) {
        bool bitReceived = detect_bit(&config);

        // Update the bit sequence with the newly received bit
        bitSequence = ((uint32_t) bitSequence << 1) | bitReceived;

        // If the start sequence '101011' is detected and it's the first time
        if ((bitSequence & sequenceMask) == expSequence && !startSequenceDetected) {
            startSequenceDetected = true;  // Mark that we found the start sequence

            int binary_msg_len = 0;
            int strike_zeros = 0;

            // Process the received bits into the message
            for (int i = 0; i < MAX_BUFFER_LEN; i++) {
                binary_msg_len++;

                if (detect_bit(&config)) {
                    msg_ch[i] = '1';
                    strike_zeros = 0;
                } else {
                    msg_ch[i] = '0';
                    if (++strike_zeros >= 8 && i % 8 == 0) {
                        break;  // Stop if 8 consecutive zeros are detected (indicating end)
                    }
                }
            }

            // Ensure proper null-termination after the last valid bit
            msg_ch[binary_msg_len - 8] = '\0';

            // Convert binary to ASCII (assuming conversion function is implemented)
            int ascii_msg_len = binary_msg_len / 8;
            char msg[ascii_msg_len];
            printf("Received Message: %s\n", conv_char(msg_ch, ascii_msg_len, msg));

            // Print the accuracy (assuming check_accuracy function is defined elsewhere)
            printf("Accuracy (%%): %f\n", check_accuracy(msg_ch, ascii_msg_len) * 100);

            // Reset bitSequence after processing the message
            bitSequence = 0;

            // Reset start sequence detection flag to allow for detecting the next message
            startSequenceDetected = false;
        }
    }

    printf("Receiver finished\n");
    return 0;
}
