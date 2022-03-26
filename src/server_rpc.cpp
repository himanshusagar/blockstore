//
// Created by Himanshu Sagar on 3/26/22.
//

#include "server_rpc.h"

Status StoreRPCServiceImpl::SayRead(ServerContext* context, const ReadRequest* request, ReadResponse* response)
{
    const int output = request->address() * 2;
    std::cout << "[server] " << request->address() << " -> " << output << "\n";
    response->set_errcode(output);
    return Status::OK;
}
Status StoreRPCServiceImpl::SayWrite(ServerContext* context, const WriteRequest* request, WriteResponse* response)
{
    return Status::OK;
}
Status StoreRPCServiceImpl::SayGetLog(ServerContext* context, const LogRequest* request, LogResponse* response)
{
    return Status::OK;
}

