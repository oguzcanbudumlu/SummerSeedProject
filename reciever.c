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

#define TOPORT 14950
#define MYPORT 1043
#define LINE_SIZE 1024
#define REC_BUFF_SIZE 1500
#define MAX_IFNAMSIZ 32

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

                    for (x = 0; x < 40; x++) {
                        toRet[x] = hdr1->data[x + 1];

                    }
                    for (x = 0; x < (strlen(hdr1->data)) - 40; x++) {
                        toSize[x] = hdr1->data[x + 41];
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

int evaluatePublisherData(char* hashFromTS, struct publisher* publisherlist){

    int sfd;
    char ifname[MAX_IFNAMSIZ] = {0};
    int ret;
    int size;

    char *buffer;
    char *arg_data;
    char *arg_ifname;
    struct sockaddr_in addrout;

    sfd = net_create_raw_socket(ifname, ETHER_TYPE, 0);
    if (sfd == -1) {
        fprintf(stderr, "failed to init socket\n");
        exit(1);
    }

    struct special_hdr *hdr1;
    arg_data = hashFromTS;
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

    int y;
    for(y = 0;y<100;y++){
        ret = send(sfd, hdr1, size, 0);
        if (ret != size) {
            fprintf(stderr, "ERROR: send failed ret: %d, errno: %d\n", ret, errno);
            exit(1);
        }
        fprintf(stderr, "%d bytes sent\n", ret);


    }


    closeSocket(sfd);








}
