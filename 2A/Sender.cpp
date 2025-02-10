#include <stdint.h>
#include <openssl/aes.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/mman.h>
#include <map>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <bitset>

#define MSG_FILE "msg.txt"
#define MAX_MSG_SIZE 500
char  BitArray[MAX_MSG_SIZE*8];
char signal = 'a' ;


void charToBits(char c, char *bits) {
    for (int i = 7; i >= 0; i--) {
        bits[7 - i] = ((c >> i) & 1) + '0';   
    }
    bits[8] = '\0';  
}


void stringToBits(const char *str) {
    char bits[9];  
    int bitIndex = 0;

    
    for (int i = 0; str[i] != '\0'; i++) {
        charToBits(str[i], bits);  
        
         
        for (int j = 0; j < 8; j++) {
            BitArray[bitIndex++] = bits[j];
        }

        BitArray[bitIndex++] = ' ';
    }
    BitArray[bitIndex - 1] = '\0';  
}


void send_data(int sockfd)
{

    char bits[5] = {'0','1','1','0'};
    unsigned char buff[16];
    int n=0;

    for (;;) {
        while (bits[n]!='\0'){
        	if (BitArray[n] == '1')
        	{
        		write(sockfd,&signal, 1);
        	}
        	else {
        		
        	}
        	n++;
        	}
    } n=0;
}

int main()
{
    int sockfd, connfd;
    unsigned int len;
    struct sockaddr_in victim_addr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        // printf("socket creation failed...\n");
        exit(0);
    }
    else
         printf("socket successfully created..\n");
    bzero(&victim_addr, sizeof(victim_addr));

    victim_addr.sin_family = AF_INET;
    victim_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    victim_addr.sin_port = htons(10000);


    if ((bind(sockfd, (struct sockaddr*)&victim_addr, sizeof(victim_addr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
         printf("socket successfully binded..\n");
         
           FILE *fp = fopen(MSG_FILE, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        return 1;
    }

    char msg[MAX_MSG_SIZE];
    int msg_size = 0;
    char c;
    while((c = fgetc(fp)) != EOF){
        msg[msg_size++] = c;
    }
    fclose(fp);


    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Sender waiting for receiver to come online ..\n");
    len = sizeof(cli);

    // accept the data packet from attacker and verification
    connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
    if (connfd < 0) {
        printf("victim acccept failed...\n");
        exit(0);
    }
     else
         printf("receiver acccepted ...\n");
    

    send_data(connfd);
    close(sockfd);
}
