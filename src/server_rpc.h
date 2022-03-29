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
#include "client_rpc.h"

#define MAX_FILE_SIZE 1e11
#define pathname "/users/kkaushik/dev/foo.txt"

using namespace helloworld;
using namespace grpc;
using namespace std;

class StoreRPCServiceImpl final : public StoreRPC::Service
{

private:
    int storefd;

public:
    bool leader;
    string backup_conn;
    backup_conn.resize(256);
    int hostname = gethostname(hostbuffer.data(), hostbuffer.size());
    ChannelArguments ch_args;

    if (hostbuffer[4] == '0')
    {
        backup_conn = "128.110.219.93:50051"
    }
    else
    {
        backup_conn = "128.110.219.112:50051"
    }

    StoreRPCClient storeReplicateRpc(
        grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials(), ch_args));

    StoreRPCServiceImpl()
    {
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
    }

    Status SayRead(ServerContext *context, const ReadRequest *request, ReadResponse *response);
    Status SayWrite(ServerContext *context, const WriteRequest *request, WriteResponse *response);
    Status SayGetLog(ServerContext *context, const LogRequest *request, LogResponse *response);
};

#endif // BLOCKSTORE_SERVER_RPC_H
