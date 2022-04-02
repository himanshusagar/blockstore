//
// Created by Himanshu Sagar on 3/26/22.
//

#include "client_rpc.h"

#define MAX_SIZE 4096

int StoreRPCClient::SayRead(int in, char *data)
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
    WriteRequest req;
    req.set_address(in);
    req.set_data(data);

    WriteResponse reply;

    ClientContext context;

    Status status = stub_->SayWrite(&context, req, &reply);
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
int StoreRPCClient::PingLeader(){
    PingRequest req;
    PongResponse reply;

    req.set_request("Hello Leader, give response!");

    ClientContext context;
    Status status = stub_->HeartBeat(&context, req, &reply);
    if (status.ok())
    {
        if (reply.leader()){
            cout << "Successful ping to leader" << endl;
            return 0;
        }
        else{
            cout << "Leader seems to be in backup mode" << endl;
            // TODO handle connection problems?
        }
    }
    return -1;
}

int StoreRPCClient::PingBackup(){
    PingRequest req;
    PongResponse reply;

    req.set_request("Hello Backup, give response!");

    ClientContext context;
    Status status = stub_->HeartBeat(&context, req, &reply);
    if (status.ok())
    {
        if (reply.leader() == false){
            cout << "Successful ping to backup" << endl;
            return 0;
        }
        else{
            cout << "Backup seems to be in leader mode" << endl;
            // TODO handle connection problems?
        }
    }
    return -1;
}

