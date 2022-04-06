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
using namespace std;

enum OP
{
    OP_READ,
    OP_WRITE
};

class StoreRPCClient
{
public:
    std::string mIP;
    StoreRPCClient(std::shared_ptr<Channel> channel , std::string& my)
        : stub_(StoreRPC::NewStub(channel)) , mIP(my) { }

    int SayRead(int in, string& val);
    int SayWrite(int in, string& val);
    int SayInternalReq(OP op , int in, string val);
    int SayGetLog(int in, WriteRequest& obj);
    int PingLeader();
    int PingBackup();
    int Ping(PongResponse *reply);

private:
    std::unique_ptr<StoreRPC::Stub> stub_;
};


class Client
{
    const std::string PRIMARY_IP = "10.10.1.1";
    const std::string BACKUP_IP = "10.10.1.2";

    StoreRPCClient *primary_server;
    StoreRPCClient *back_server;
    int MAX_RETRY;
    string mPort;
public:
    Client(const std::string& port)
    {
        mPort = port;
        MAX_RETRY = 3;
        primary_server = NULL;
        back_server = NULL;
        SwitchServer();
    }
    void SwitchServer();
    void Initialize(std::string pri_str , std::string sec_str);
    int SayReq(OP op , int in, string val);


};
#endif // BLOCKSTORE_CLIENT_RPC_H