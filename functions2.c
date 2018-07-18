

//___________manager_______________________

char* retrievePast(file* fp)
//yarıda kalan dosyaların verilerinin getirilmesi
//return dosya adı + geçmiş buffer



void printScreen(char* ch)
//ekrana sistem durumunu bastır

int createReciever(struct reciever** clientList, char* clientName)
//alınacak her yeni dosya için client structu oluşturup listeye ekler

int createPublisher(struct Publisher** publisherList, char* publisherName)
//yollanıcak her yeni dosya için publisher structu oluşturup listeye ekler

int listen(char* fileName)
//listens for user input and network requests

int deleteClient(struct Client** clientList, char* clientName)

int deletePublisher(struct Publisher** publisherList, char* publisherName)

int notifyUserNoPublisher(char* fileName)
//notify user that no one sending packets from specific file

//__________reciever______________

int createSocket(struct sockaddr_in sockaddr)

int closeSocket(struct sockaddr_in sockaddr)

char* requestHash(char* fileName, int* size,file* trustedSourceFile)
//requests hash code from trusted source
//return hash code and file size

int evaluatePublisherData(int hashFromTS, struct publisher* publisherList)
//send hashcode from trusted source to network
//return # of publisher and their info


partition(int size, int numberOfPublishers, struct publisher* publisherList)
//partition the file in any state of downloading
//called every 3 mins to refresh network info 


askForPartitions(struct publisher publisherList)
//tell publisher their partitions and request them

int evaluateHash(char* hashFromTS, struct publisher pd)
//calculate if incoming data is correct
//return 0 if ok 1 if not

void pause(struct publisher* publisherList)
//stops recieving and notifies publishers

void sendBufferToPlayer(no fooking idea)
//indirme devam ederken oynatma aksiyonu 

int downloadFinishSignal()
//notifies manager and publishersthat the download is complete

int noPublishers()
//notifies manager that no publisher left in network

int addToBlackList(file* blackList, struct publisher* publisher)
//if smo sent a invalid packet take em to blacklist


void write_to_file(int *sizes, char **fragments, char *fname, int fragsize)


int addPublisher(struct publisherData* publisherList)


int removePublisher(struct publisherData* publisherList)


int sendHashToLocalPublisher(char* finalHash)
//sent hashes retrieved from another source to local publisher

//__________publisher______________


int createSocket(struct sockaddr_in sockaddr)

int closeSocket(struct sockaddr_in sockaddr)


int check_if_file_exists(char* hash, file* fp);
//checks local shared folder for file requested

int validate_if_file_exists(int result_of_checking,struct sockaddr_in ip,struct fileStatus* fs);
//sends if file exist and if it is the status of file(full or still downloading if downloading existing parts)

int calculate_hash(const file* fp, int numberOfFragments, struct hashPacket* hp);
//takes numberOfFragment and returns the hashPacket

int calculateHashFileNotReady(const file* fp, char* hash)
//if file still downloading sends the required hashs to reciever


int requestHashFromLocalReciever(char* finalHash)
//for an unfinished file asks local reciever the hashes retrived from remote publisher

int getPartitionInfo()
//recieve packet numbers to be sent

int send_hash(char* hash);
//send hash information to reciever

int send(char* buf)
//send actual data packets

int finished()
//after reciever validates downloading complete notify manager and close publisher



//_________structs____________

struct Reciever{
    
    struct Publisher* publisherList;//for reciever
    char* name;//for publisher
    int dataSize;//for reciever
    struct fileStatus;
        
    
}
struct Publisher{

    char* name;
    struct sockaddr_in;
    uint16_t startingIndex;
    uint16_t finishIndex;
    char** md5Array;


}



struct fileStatus{

    int* dataStartingIndexes;
    int* dataFinishedIndexes;
    int finished;
}

packets:
struct reciever_to_publisher_packet_hdr {

    uint16_t startingIndex;
    uint16_t finsishIndex;
    char isFinished;
    uint16_t fragSize;
    }__attribute__((packed));
    
struct publisher_to_reciever_packet_hdr{

    uint16_t frindex;
    char packetType;
 
}__attribute__((packed))
    
    








