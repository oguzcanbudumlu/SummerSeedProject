//
// Created by alican on 17.07.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <errno.h>
#include <libgen.h>
#include <time.h>
#include <netpacket/packet.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include "publisher.h"
#include "packet_headers.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include "myether.h"
#include <time.h>
#include <unistd.h>

#define TOPORT 14950
#define MYPORT 1043
#define LINE_SIZE 1024
#define REC_BUFF_SIZE 1500
#define MAX_IFNAMSIZ 32
#define FRAG_SIZE 1500

struct special_hdr{
    struct reciever_to_publisher_packet_hdr hdrr;
    struct ether_header eth;
}__attribute__((packed));

void my_ip(char *myniccard, char *myipaddr) {
    int fd;
    struct ifreq ifr;

    myipaddr[0] = 0;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    //strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    strncpy(ifr.ifr_name, myniccard, IFNAMSIZ - 1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
    sprintf(myipaddr, "%s", inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr));
    printf("MY IP address:%s: on port: %d\n", myipaddr, MYPORT);

}


int createSocket(struct sockaddr_in *sockaddr) {

    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("Error opening socket\n");
        return -1;
    }

    sockaddr.sin_port = htons(PORT);
    sockaddr.sin_addr.s_addr = 0;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_family = AF_INET;

    if (bind(fd, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr_in)) == -1) {
        printf("Error binding socket\n");
        return -1;
    }

    return fd;
}

int closeSocket(/*struct sockaddr_in* sockaddr,*/ int fd) {
    close(fd);
}

int requestHash(char *fileName) {
    FILE *file = fopen("files/TrustedSourceFile.txt", "r");
    char myipaddressm[22];
    char *myniccardm = "eth0";
    struct sockaddr_in localaddr;
    char line[LINE_SIZE];
    int sockfd;
    struct sockaddr_in their_addr;
    struct reciever_to_publisher_packet_hdr hdr1;
    struct hostent *he;

    assert(file != NULL);
    my_ip(myniccardm, myipaddressm);


    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Sender: Client-socket() error lol!");
        exit(1);
    }

    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = inet_addr(myipaddressm);
    localaddr.sin_port = htons(MYPORT);
    bind(sockfd, (struct sockaddr *) &localaddr, sizeof(localaddr));


    while (fgets(line, sizeof(line), file)) {

        if ((he = gethostbyname(line)) == NULL) {
            perror("Sender: Client-gethostbyname() error!");
            exit(1);
        }


        their_addr.sin_family = AF_INET;
        their_addr.sin_port = htons(TOPORT);
        their_addr.sin_addr = *((struct in_addr *) he->h_addr);
        memset(&(their_addr.sin_zero), '\0', 8);

        hdr1.startingIndex = 0;
        hdr1.finishIndex = 0;
        hdr1.fragsize = 0;
        hdr1.isFinished = '0';
        hdr1.data = fileName;
        char *toSent = (char *) (hdr1);

        if ((numbytes = sendto(sockfd, toSent,
                               strlen(toSent),
                               0,
                               (struct sockaddr *) &their_addr,
                               sizeof(struct sockaddr))) == -1) {
            perror("Sender: Client-sendto() error lol!");
            exit(1);
        } else
            printf("Sender: Client-sendto() is OK...\n");
    }
    if (close(sockfd) != 0)
        printf("Sender: Client-sockfd closing is failed!\n");
    else
        printf("Sender: Client-sockfd successfully closed!\n");

}

char *recieveHashFromTS(char *fileName, int *size) {

    FILE *file = fopen("files/TrustedSourceFile.txt", "r");
    struct sockaddr_in recaddr;
    int *sfd;
    char ifname[MAX_IFNAMSIZ] = {0};
    int ret;
    char *buffer;
    char *arg_ifname;
    struct publisher_to_reciever_packet_hdr *hdr1;
    int lines;
    int ch;
    char *toRet;
    ch = 0;
    lines = 0;
    int i;
    char *toSize;
    if (fp == NULL)
        return 0;

    while (!feof(fp)) {
        ch = fgetc(fp);
        if (ch == '\n') {
            lines++;
        }
    }

    struct pollfd ufds[lines];

    for (i = 0; i < lines; i++) {
        sfd[i] = createSocket(recaddr);
        ufds[i].fd = sfd[i];
        ufds[i].events = POLLIN;
    }

    int recieved;
    recieved = 0;
    while (recieved < lines) {
        int pp = poll(ufds, lines, POLL_TIMEOUT);
        if (pp == -1) {
            perror("poll");
            continue;
        }

        for (i = 0; i < lines; i++) {
            if (ufds[i].revents & POLLIN) {
                ret = recv(sfd, hdr1, REC_BUFF_SIZE, 0);
                if (ret <= 0) {
                    fprintf(stderr, "ERROR: recv failed ret: %d, errno: %d\n", ret, errno);
                    exit(1);
                }

                if (hdr1->data[0] == '1') {
                    int x;

                    for (x = 0; x < 32; x++) {
                        toRet[x] = hdr1->data[x + 1];

                    }
                    for (x = 0; x < (strlen(hdr1->data)) - 32; x++) {
                        toSize[x] = hdr1->data[x + 33];
                    }
                    size = (int) (toSize);
                    recieved = lines;
                }

            }
        }
        recieved++;
    }
    if(toRet == NULL)
        return "file not found";
    return toRet;



}

int broadCastMD5(char* hashFromTS, struct publisher* publisherlist){

    int sfd;
    char ifname[MAX_IFNAMSIZ] = {0};
    int ret;
    int size;
    char *buffer;
    char *arg_data;
    char *arg_ifname;
    char *myniccardm = "eth0";
    char myipaddressm[22];
    struct sockaddr_in addrout;

    time_t endwait;
    time_t start = time(NULL);
    time_t seconds = 10;
    endwait = start + seconds;

    sfd = net_create_raw_socket(ifname, ETHER_TYPE, 0);
    if (sfd == -1) {
        fprintf(stderr, "failed to init socket\n");
        exit(1);
    }

    struct special_hdr *hdr1;
    arg_data = hashFromTS + myipaddressm;
    size = strlen(arg_data) + 1 + sizeof(*hdr1);

    buffer = malloc(size);
    if (!buffer){
        fprintf(stderr, "memory allocation error!\n");
        exit(1);
    }
    hdr1 = (struct special_hdr*)buffer;

    net_get_iface_mac(ifname, (char *)hdr1->eth.ether_shost);
    hdr1->eth.ether_type = htons(ETHER_TYPE);
    memcpy(hdr1->eth.ether_dhost, ETH_ADD_BCAST, ETH_ALEN);
    hdr1->hdrr.startingIndex = 0;
    hdr1->hdrr.finishIndex = 0;
    hdr1->hdrr.fragsize = 0;
    hdr1->hdrr.isFinished = 'c';
    strcpy(hdr1->hdrr.data, arg_data);

    endwait = start + seconds;
    while(start < endwait){
        ret = send(sfd, hdr1, size, 0);
        if (ret != size) {
            fprintf(stderr, "ERROR: send failed ret: %d, errno: %d\n", ret, errno);
            exit(1);
        }
        fprintf(stderr, "%d bytes sent\n", ret);

        start = time(NULL);
    }


    closeSocket(sfd);


}

int recievePublishers(struct publisher* publisherList,int* numberOfPublishers){
    int sfd;
    char ifname[MAX_IFNAMSIZ] = {0};
    int ret;
    struct publisher_to_reciever_packet_hdr *hdr1;
    char* buffer;
    char *arg_ifname;
    struct sockaddr_in inaddr;
    numberOfPublishers = 0;
    sfd =  createSocket(inaddr);
    buffer = malloc(REC_BUFF_SIZE);
    hdr1 = (struct publisher_to_reciever_packet_hdr*) buffer;
    time_t endwait;
    time_t start = time(NULL);
    time_t seconds = 10;
    endwait = start + seconds;
    int i;
    i = 0;
    while(start < endwait) {
        ret = recv(sfd, hdr1, REC_BUFF_SIZE, 0);

        if(hdr1->packetType =='r') {
            publisherList[i] = (struct publisher *) calloc(1, sizeof(struct publisher));
            memcpy(publisherList[i]->name, hdr1->data, 23);
            createSocket(publisherList[i]->sockaddr_in);
            i++;

        }
        start = time(NULL);
    }



    numberOfPublishers = i;




}

int partition(int size,int numberOfPublishers,  struct publisher* publisherList){

    int sizePerPublisher;
    sizePerPublisher = size/numberOfPublishers;
    int remaining;
    if(sizePerPublisher != size){
        remaining = size - (sizePerPublisher*numberOfPublishers);
    }

    int i;
    for(i = 0; i < numberOfPublishers; i++){
        publisherList[i]->startingIndex = i*sizePerPublisher;
        publisherList[i]->finishIndex = (i+1)*sizePerPublisher;
        if(publisherList[i]->finishIndex > size/FRAG_SIZE){
            publisherList[i]->finishIndex = size/FRAG_SIZE ;
        }

    }

}

int askForPartitions(int numberOfPublishers,struct publisher* publisherlist){

    struct reciever_to_publisher_packet_hdr *hdrr;
    int sockfd;
    struct sockaddr_in their_addr;
    int i;
    struct hostent *he;

    for(i = 0; i<numberOfPublishers;i++){
        hdrr->startingIndex = publisherlist[i]->startingIndex;
        hdrr->finishIndex = publisherlist[i]->finishIndex;
        hdrr->isFinished = '0';
        char *toSent = (char *) (hdrr);




        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("Sender: Client-socket() error lol!");
            exit(1);
        }

        localaddr.sin_family = AF_INET;
        localaddr.sin_addr.s_addr = inet_addr(myipaddressm);
        localaddr.sin_port = htons(MYPORT);
        bind(sockfd, (struct sockaddr *) &localaddr, sizeof(localaddr));


        their_addr.sin_family = AF_INET;
        their_addr.sin_port = htons(TOPORT);
        their_addr.sin_addr = *((struct in_addr *) he->h_addr);
        memset(&(their_addr.sin_zero), '\0', 8);

        if ((numbytes = sendto(sockfd, toSent,
                               strlen(toSent),
                               0,
                               (struct sockaddr *) &their_addr,
                               sizeof(struct sockaddr))) == -1) {
            perror("Sender: Client-sendto() error lol!");
            exit(1);
        }
        if (close(sockfd) != 0)
            printf("Sender: Client-sockfd closing is failed!\n");




    }

}

int evaluateHash(char* hashFromTS, struct publisher* p){


}


void recieveHashPacket(int numberOfPublishers,struct publisher* publisherlist){

    int sfd;
    char ifname[MAX_IFNAMSIZ] = {0};
    int ret;
    struct publisher_to_reciever_packet_hdr *hdr1;
    char* buffer;
    char *arg_ifname;
    struct sockaddr_in inaddr;
    numberOfPublishers = 0;
    sfd =  createSocket(inaddr);
    buffer = malloc(REC_BUFF_SIZE);
    hdr1 = (struct publisher_to_reciever_packet_hdr*) buffer;
    int i,y,z,t;
    for(i = 0; i<numberOfPublishers;i++){
        ret = recv(sfd, hdr1, REC_BUFF_SIZE, 0);
        for(y=0;y<numberOfPublishers;y++){
            char checker[22];
            for(z=0;z<22;z++){
                checker[i] = hdr1->data[i];
            }

            if(strcmp(checker,publisherlist[i]->name)){

                for(z = 0; hdr1->data[0];z++){
                    for(t=0;t<22;t++){
                        publisherlist[z]->md5Array[z][t] = hdr1->data[1+z*22+t];
                    }
                }

            }
        }
    }
}

int recievePackets(struct publisher* p, int fragmentCount,char* hashFromTS,char* filename){

    int fd;
    struct sockaddr_in insock;
    fd = createSocket(insock);
    struct publisher_to_reciever_packet_hdr hdr;
    if (fd == -1) {
        fprintf(stderr, "failed to init socket\n");
        exit(1);
    }


    int ret;
    char* buffer;
    buffer = malloc(RX_BUF_SIZE);
    if (!buffer) {
        fprintf(stderr, "memory allocation error!\n");
        exit(1);
    }

    hdr = (struct publisher_to_reciever_packet_hdr*)buffer;
    char **indexing;
    int *arr;
    arr = calloc(hdr->fragmentcount,sizeof(int));
    indexing = calloc(hdr->fragmentcount,sizeof(char *));
    int i;
   for(i=0;i<fragmentCount;i++){
        ret = recv(fd, hdr, RX_BUF_SIZE, 0);
        if (ret <= 0) {
            fprintf(stderr, "ERROR: recv failed ret: %d, errno: %d\n", ret, errno);
            exit(1);
        }
        indexing[hdr.frindex - 1] = (char *) calloc(hdr->size,sizeof(char));
        memcpy(indexing[hdr.frindex -1],hdr->data,hdr->size);
        arr[hdr.frindex-1] =hdr->size;

    }
    if((i = evaluateHash(hashFromTS, p)) == 0){
       write_to_file(arr,indexing,filename,fragmentCount);
   }


    free(hdr2);
}