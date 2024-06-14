#ifndef SOCKET_H_
#define SOCKET_H_
namespace serversock {
    
    struct objectData {
        unsigned int value;
    };
    
    void createConnection();
    int readValues(objectData *a);

}
#endif