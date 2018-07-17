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
    }__attribute__((packed));

    struct publisher_to_reciever_packet_hdr{

        uint16_t frindex;
        char packetType;


    }__attribute__((packed));




#endif //SUMMERSEEDPROJECT_PACKET_HEADERS_H
