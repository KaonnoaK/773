#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/mman.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include "./cacheutils.h"
#include <map>
#include <vector>
#include <iostream>
#include <bitset>
#include <sys/socket.h>
#include <iomanip>

#define MIN_CACHE_MISS_CYCLES (400)
#define MSG_FILE "msg.txt"
#define MAX_MSG_SIZE 500
char  BitArray[MAX_MSG_SIZE*8];
int n = 0;

bool debug_flag = false;

void connect_to_sender(int &sockfd)
{
    struct sockaddr_in victim_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }

    bzero(&victim_addr, sizeof(victim_addr));

    victim_addr.sin_family = AF_INET;
    victim_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    victim_addr.sin_port = htons(10000);

    if (connect(sockfd, (struct sockaddr*)&victim_addr, sizeof(victim_addr)) != 0) {
        printf("connection with the victim failed...\n");
        exit(0);
    }
     else
     printf("connected to the victim..\n");
}

int main(int argc, char *argv[])
{
    int NUMBER_OF_ENCRYPTIONS;
    if (argc < 2) {
        printf("Usage: ./attacker <iterations>\n");
        exit(1);
    }
    else
        NUMBER_OF_ENCRYPTIONS = atoi(argv[1]);
        
    
    char reading[NUMBER_OF_ENCRYPTIONS][MAX_MSG_SIZE*8];
    
    for (int i = 0; i < NUMBER_OF_ENCRYPTIONS; i++) {
    	for (int j = 0 ; j< MAX_MSG_SIZE ; j++)
    		reading[i][j] = '0';
    	}
    
    printf(" \n %s \n ",reading[1]);
       
    int sockfd;
    connect_to_sender(sockfd);

    int fd = open("./sender", O_RDONLY);

    size_t size = lseek(fd, 0, SEEK_END);
    if (size == 0)
        exit(-1);

  
    size_t map_size = size;
  
    if (map_size & 0xFFF != 0) {
        map_size |= 0xFFF;
        map_size += 1;
    }
      char *base = (char *)mmap(0, map_size, PROT_READ, MAP_SHARED, fd, 0);
    char *probe = base;
    uint64_t min_time = rdtsc();
    srand(min_time);

    printf("====================================================\n");
    
    debug_flag = true;
    for (int i = 0; i < NUMBER_OF_ENCRYPTIONS; ++i) {
         
        while (n<8) { 
        if (debug_flag)
            printf("Flusing the base address ... .. ...\n");
        flush(probe);
 
        if (debug_flag)
            printf("Timing the probe ..\n");
            
        size_t time = rdtsc();
        maccess(probe); 
        size_t delta = rdtsc() - time;
        
        printf(" \n \n %lld cyccles of delta .. \n",delta);
        if (delta > MIN_CACHE_MISS_CYCLES)
        {
            reading[i][n] = '0';
        }
        else {
        	reading[i][n] = '1';
        }
        
        n++;
        }
        n=0;
        
       // debug_flag = false;
    }
    printf("====================================================\n");
    
    for (int j = 0; j< NUMBER_OF_ENCRYPTIONS;j++) {
   
    for (int i = 0; i< 8; i++) {
    
    printf(" %c  ",reading[j][i]);
    } printf("\n \n");}

    close(fd);
    munmap(base, map_size);
    fflush(stdout);
    return 0;
}
