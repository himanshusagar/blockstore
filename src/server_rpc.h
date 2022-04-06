//
// Created by Himanshu Sagar on 3/26/22.
//

#ifndef BLOCKSTORE_SERVER_RPC_H
#define BLOCKSTORE_SERVER_RPC_H

#include "blockstore.grpc.pb.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "client_rpc.h"
#include <deque>
#include <unordered_map>
#include <semaphore.h>
#include "crash_points.h"

#define MAX_FILE_SIZE 1e11
#define pathname "/users/hsagar/dev/akshat/foo.txt"

using namespace helloworld;
using namespace grpc;
using namespace std;

class Request
{
public:
    int address;
    const char *data;
};

class StoreRPCServiceImpl final : public StoreRPC::Service
{

private:
    int storefd;

public:
    StoreRPCClient *connOtherServer;
    // sem_t mutex;
    pthread_mutex_t mp;
    pthread_cond_t cv;
    int retries = 3;
    bool leader;
    bool backupIsActive;
    char hostbuffer[256];
    int failed_heartbeats;
    int maxRetry;
    int hostname = gethostname(hostbuffer, 256);
    bool replication;
    std::string currPhase;
    deque<const WriteRequest *> request_queue;
    unordered_map<int, Request *> requestMap;
    ChannelArguments ch_args;

    StoreRPCServiceImpl(string &backup_str)
    {
        
        storefd = open(pathname, O_RDWR, S_IRUSR | S_IWUSR);
        if (storefd < 0)
        {
            // if open failed, create file
            storefd = open(pathname, O_CREAT, S_IRUSR | S_IWUSR);
            if (storefd < 0)
            {
                cout << "File Creation Failed";
            }
            int result = fallocate(storefd, 0, 0, MAX_FILE_SIZE);
            if (result == -1)
            {
                cout << "File Allocation Failed";
            }
        }
        maxRetry = 16;
    } 
    
    int PerformRecovery();
    Status SayRead(ServerContext *context, const ReadRequest *request, ReadResponse *response);
    Status SayWrite(ServerContext *context, const WriteRequest *request, WriteResponse *response);
    Status SayGetLog(ServerContext *context, const LogRequest *request, LogResponse *response);
    Status HeartBeat(ServerContext *context, const PingRequest *request, PongResponse *response);
};

#endif // BLOCKSTORE_SERVER_RPC_H
