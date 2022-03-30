//
// Created by Himanshu Sagar on 3/26/22.
//

#include "server_rpc.h"
#include <unistd.h>

#define MAX_SIZE 4096

using grpc::Status;

Status StoreRPCServiceImpl::SayRead(ServerContext *context, const ReadRequest *request, ReadResponse *response)
{
    int address = request->address();
    char buff[MAX_SIZE];
    lseek(storefd, address, SEEK_SET);
    int result = read(storefd, buff, MAX_SIZE);
    response->set_data(buff);
    if (result == -1)
    {
        response->set_errcode(errno);
    }
    else
    {
        response->set_errcode(0);
    }
    return Status::OK;
}

Status StoreRPCServiceImpl::SayWrite(ServerContext *context, const WriteRequest *request, WriteResponse *response)
{

    int address = request->address();
    lseek(storefd, address, SEEK_SET);

    // Main Action
    cout << "Write" << endl;
    int result = write(storefd, request->data().data(), MAX_SIZE);
    if (result == -1)
    {
        cout << "Write Successfull" << endl;
        response->set_errcode(errno);
    }
    // Checking if the current instance is primary
    if (leader)
    {
        // Sending to the primary backup
        int rep_result = storeReplicateRpc->SayWrite(address, request->data().data());
        if (rep_result != 0)
        {
            cout << rep_result;
            response->set_errcode(rep_result);
            cout << "Replication on Backup is failed" << endl;
        }
        else
        {
            cout << "Replication on Backup is successfull" << endl;
        }
    }

    response->set_errcode(0);

    return Status::OK;
}

Status StoreRPCServiceImpl::SayGetLog(ServerContext *context, const LogRequest *request, LogResponse *response)
{
    return Status::OK;
}
