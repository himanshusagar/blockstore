//
// Created by Himanshu Sagar on 3/26/22.
//

#ifndef BLOCKSTORE_CLIENT_RPC_H
#define BLOCKSTORE_CLIENT_RPC_H

#include "blockstore.grpc.pb.h"
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <chrono>
#include <ctime>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <fstream>
#include <memory>

using namespace helloworld;
using namespace grpc;

class StoreRPCClient
{
public:
    StoreRPCClient(std::shared_ptr<Channel> channel)
        : stub_(StoreRPC::NewStub(channel)) {}

    int SayRead(int in, char *data);
    int SayWrite(int in, char *data);

private:
    std::unique_ptr<StoreRPC::Stub> stub_;
};

#endif // BLOCKSTORE_CLIENT_RPC_H