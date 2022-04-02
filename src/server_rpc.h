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

#define MAX_FILE_SIZE 1e11
#define pathname "/users/kkaushik/dev/foo.txt"

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
    int retries = 3;
    bool leader;
    bool backupIsActive;
    char hostbuffer[256];
    int failed_heartbeats;
    int maxRetry;
    int hostname = gethostname(hostbuffer, 256);
    std::string currPhase;
    deque<Request *> request_queue;
    unordered_map<int, Request *> requestMap;
    StoreRPCClient *storeReplicateRpc;

    StoreRPCServiceImpl(string &backup_str, string &phase)
    {
        currPhase = "start";
        const std::string target_str = "localhost:50051";
        grpc::ChannelArguments ch_args;
        ch_args.SetMaxReceiveMessageSize(INT_MAX);
        ch_args.SetMaxSendMessageSize(INT_MAX);

        storeReplicateRpc = new StoreRPCClient(
            grpc::CreateCustomChannel(backup_str, grpc::InsecureChannelCredentials(), ch_args));

        storefd = open(pathname, O_RDWR, S_IRUSR | S_IWUSR);
        if (storefd < 0)
        {
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

    int backupRecovery();
    int leaderShift();

    Status SayRead(ServerContext *context, const ReadRequest *request, ReadResponse *response);
    Status SayWrite(ServerContext *context, const WriteRequest *request, WriteResponse *response);
    Status SayGetLog(ServerContext *context, const LogRequest *request, LogResponse *response);
    Status HeartBeat(ServerContext *context, const PingRequest *request, PongResponse *response);
};

#endif // BLOCKSTORE_SERVER_RPC_H
