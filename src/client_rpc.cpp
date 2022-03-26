//
// Created by Himanshu Sagar on 3/26/22.
//

#include "client_rpc.h"

int StoreRPCClient::SayRead(int in)
{
    // Data we are sending to the server.
    ReadRequest req;
    req.set_address(in);

    // Container for the data we expect from the server.
    ReadResponse reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.

    Status status = stub_->SayRead(&context, req, &reply);
    // Act upon its status.
    if (status.ok())
    {
        return reply.errcode();
    }
    else
    {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return -1;
    }
}