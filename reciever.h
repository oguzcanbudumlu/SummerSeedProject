//
// Created by alican on 17.07.2018.
//

#ifndef SUMMERSEEDPROJECT_RECIEVER_H
#define SUMMERSEEDPROJECT_RECIEVER_H

    struct reciever{

    struct publisher* publisherlist;
    int dataSize;
    char* name;
    struct fileStatus;

};

    int createSocket(struct sockaddr_in sockaddr);
    int closeSocket(struct sockaddr_in sockaddr_in);
    int requestHash(char* fileName);
    char* recieveHashFromTS(char* fileName, int* size);
    int evaluatePublisherData(char* hashFromTS, struct publisher* publisherlist);
    int partition(int size, int numberOfPublishers, struct publisher* publisherlist);
    int askForPartitions(struct publisher* publisherlist);
    int evaluateHash(char* hashFromTS, struct publisher* p);
    void pause(struct publisher* publisherlist);
    void sendBufferToPlayer();
    int downloadFinishSignal();
    int noPublishers();
    int addToBlackList(file* blackList, struct publisher* p);
    void write_to_file(int *sizes, char **fragments, char *fname, int fragsize);
    int addPublisher(struct publisher* publisherList);
    int removePublisher(struct publisher* publisherlist, struct publisher p);
    void my_ip( char *myniccard, char *myipaddr);

#endif //SUMMERSEEDPROJECT_RECIEVER_H
