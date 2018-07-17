//
// Created by alican on 17.07.2018.
//

#ifndef SUMMERSEEDPROJECT_PACKET_HEADERS_H
#define SUMMERSEEDPROJECT_PACKET_HEADERS_H

    struct reciever_to_publisher_packet_hdr {
        uint16_t startingIndex;
        uint16_t finishIndex;
        uint16_t fragsize;
        char isFinished;
        char data[0];
    }__attribute__((packed));

    struct publisher_to_reciever_packet_hdr{

        uint16_t frindex;
        char packetType;
        char data[0];

    }__attribute__((packed));

    struct fileStatus{

        int* dataStartingIndexes;
        int* dataFinishedIndexes;
        int finished;

    };


#endif //SUMMERSEEDPROJECT_PACKET_HEADERS_H
