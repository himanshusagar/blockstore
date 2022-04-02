//
// Created by Himanshu Sagar on 3/26/22.
//

#include "client_rpc.h"
#include <time.h>

#define BILLION (long long int)1000000000

#define MAX_SIZE 4096

int StoreRPCClient::SayRead(int in, char *data)
{

    struct timespec start, end, mid;
    long long int diff;

    // Data we are sending to the server.
    ReadRequest req;
    req.set_address(in);

    // Container for the data we expect from the server.
    ReadResponse reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    clock_gettime(CLOCK_MONOTONIC, &start);
    Status status = stub_->SayRead(&context, req, &reply);
    clock_gettime(CLOCK_MONOTONIC, &end);

    diff = BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
    std::cout << diff << std::endl;

    // Act upon its status.
    if (status.ok())
    {
        if (reply.errcode() == 0)
        {
            memcpy(data, reply.data().data(), MAX_SIZE);
            return 0;
        }
        else
        {
            return reply.errcode();
        }
    }
    else
    {
        std::cout << status.error_code() << std::endl;
        return -1;
    }
}

int StoreRPCClient::SayWrite(int in, const char *data)
{
    struct timespec start, end, mid;
    long long int diff;

    WriteRequest req;
    req.set_address(in);
    req.set_data(data);

    WriteResponse reply;

    ClientContext context;

    clock_gettime(CLOCK_MONOTONIC, &start);
    Status status = stub_->SayWrite(&context, req, &reply);
    clock_gettime(CLOCK_MONOTONIC, &end);

    diff = BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
    std::cout << diff << std::endl;
    if (status.ok())
    {
        if (reply.errcode() == 0)
        {
            return 0;
        }
        else
        {
            return reply.errcode();
        }
    }
    else
    {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return -1;
    }
}
