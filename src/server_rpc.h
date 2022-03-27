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

using namespace helloworld;
using namespace grpc;

class StoreRPCServiceImpl final : public StoreRPC::Service
{

private:
    int storefd;

public:
    StoreRPCServiceImpl()
    {
        storefd = open("foo.txt", O_RDWR);
    }

    Status SayRead(ServerContext *context, const ReadRequest *request, ReadResponse *response);
    Status SayWrite(ServerContext *context, const WriteRequest *request, WriteResponse *response);
    Status SayGetLog(ServerContext *context, const LogRequest *request, LogResponse *response);
};

#endif // BLOCKSTORE_SERVER_RPC_H
