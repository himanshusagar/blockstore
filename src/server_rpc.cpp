//
// Created by Himanshu Sagar on 3/26/22.
//

#include "server_rpc.h"
#include <unistd.h>

#define MAX_SIZE 4096

using grpc::Status;

Status StoreRPCServiceImpl::SayRead(ServerContext *context, const ReadRequest *request, ReadResponse *response)
{
    int address = request->address;
    char buff[MAX_SIZE];
    lseek(storefd, address, SEEK_SET);
    read(storefd, buff, MAX_SIZE);
    response.set_data(buff);
    if (result == -1)
    {
        response.set_errcode(errno);
    }
    else
    {
        response.set_errcode(0);
    }
    return Status::OK;
}

Status StoreRPCServiceImpl::SayWrite(ServerContext *context, const WriteRequest *request, WriteResponse *response)
{
    int address = request->address;
    char buff[MAX_SIZE] = request->data;
    lseek(storefd, address, SEEK_SET);
    int result = write(storefd, buff, MAX_SIZE);
    if (result == -1)
    {
        response.set_errcode(errno);
    }
    else
    {
        response.set_errcode(0);
    }
    return Status::OK;
}

Status StoreRPCServiceImpl::SayGetLog(ServerContext *context, const LogRequest *request, LogResponse *response)
{
    return Status::OK;
}
