  #include "utils.h"

// DO NOT MODIFY THIS FUNCTION
double check_accuracy(char* received_msg, int received_msg_size){
    FILE *fp = fopen(MSG_FILE, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        return 1;
    }

    char original_msg[MAX_MSG_SIZE];
    int original_msg_size = 0;
    char c;
    while((c = fgetc(fp)) != EOF){
        original_msg[original_msg_size++] = c;
    }
    fclose(fp);

    int min_size = received_msg_size < original_msg_size ? received_msg_size : original_msg_size;

    int error_count = (original_msg_size - min_size) * 8;
    for(int i = 0; i < min_size; i++){
        char xor_result = received_msg[i] ^ original_msg[i];
        for(int j = 0; j < 8; j++){
            if((xor_result >> j) & 1){
                error_count++;
            }
        }
    }

    return 1-(double)error_count / (original_msg_size * 8);
}


CYCLES mem_access(ADDR_PTR *addr)
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

extern inline __attribute__((always_inline))
void clflush(ADDR_PTR addr)
{
    asm volatile ("clflushopt (%0)"::"r"(addr));
}


extern inline __attribute__((always_inline))
CYCLES rdtscp(void) {
	CYCLES cycles;
	asm volatile ("rdtscp"
	: /* outputs */ "=a" (cycles));

	return cycles;
}

inline CYCLES get_time() {
    return rdtscp();
}

extern inline __attribute__((always_inline))
CYCLES cc_sync() {
    while((get_time() & CHANNEL_SYNC_TIMEMASK) > CHANNEL_SYNC_JITTER) {}
    return get_time();
}

char *string_to_binary(char *s)
{

//printf("\n \n inside string to binary \n\n");
    if (s == NULL) return 0; 

    size_t len = strlen(s) - 1;


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
    
    //printf("\n \n exiting string to binery \n\n");

    return binary;
}

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

uint64_t get_cache_set_index(ADDR_PTR phys_addr)
{
    uint64_t mask = ((uint64_t) 1 << 16) - 1;
    return (phys_addr & mask) >> 6;
}

void fill_llc (uint64_t *base) {
	uint64_t k=0, min=0,max=1048576;
	for (size_t i = 0; i < N  ; i++  ) {
		mem_access(base);base++;
		}
}
void pointer_chase(Node *head) {
    if (head == NULL)
        return;

    Node *temp = head;
    while (temp != NULL) {
        temp = temp->next;
    }
}


void create_ll(Node **head, uint64_t iter) {
    Node *curr, *temp;
    uint64_t data = 0; // Initialize data to 0 or some value

    // Allocate memory for the first node
    *head = (Node*)malloc(sizeof(Node));
    (*head)->data = data;  // Initialize the first node's data
    (*head)->next = NULL;   // The first node's next is NULL because it's the start of the list

    temp = *head;

    // Create the rest of the nodes
    for (size_t i = 2; i <= iter; i++) {
        curr = (Node*)malloc(sizeof(Node));  // Allocate memory for the next node
        curr->data = 1;                      // Set the data for the node (could be anything)
        curr->next = NULL;                   // The next pointer of this node is NULL (last node)

        temp->next = curr;  // Link the previous node to this new node
        temp = curr;        // Move temp to the newly created node (for the next iteration)
    }
}


/*void init_configS(struct config *config, int argc, char **argv)
{
		//printf("\n\n inside the init_configure \n");
		
			Node *head = create_ll();
			if(!head)
				return ;

		config->interval = CHANNEL_SENDING_INTERVAL;
	
		config->addr = head;
		
		//printf("\n\n exiting the init_configure \n");

}

void init_configR(struct config *config, int argc, char **argv)
{
		//printf("\n\n inside the init_configure \n");
		
		
		uint8_t *cache_mem = (uint8_t *)malloc(CACHE_SIZE);
		
		if(!cache_mem)
			printf("\n\n malloc ka error \n");
		//ADDR_PTR *addr = cache_mem;
		

		config->interval = CHANNEL_SENDING_INTERVAL;
	
		config->addr = cache_mem;
		
		//printf("\n\n exiting the init_configure \n");

}*/


bool detect_bit(Node *addr)
{
    size_t accesses = 0;
    size_t total_accesses = 0;  // To count accesses for N times

    CYCLES start_t = cc_sync();
    while ((get_time() - start_t) < INTERVAL) {
        for (size_t i = 0; i < 5; i++) {
            pointer_chase(addr);
            accesses++;
        }
        total_accesses += accesses;
        accesses = 0;  // Reset for next N accesses
    }
    
    printf("\n\n\t\t%ld\n", total_accesses);

    // Check if detected 1 or 0
    if (total_accesses >= MIN_ACCESSES) {
        printf("\n \t 1 ");
        return false;  // Bit is 1
    } else {
        printf("\n \t 0 ");
        return true;  // Bit is 0
    }
}

