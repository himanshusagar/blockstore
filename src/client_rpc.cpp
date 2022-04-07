//
// Created by Himanshu Sagar on 3/26/22.
//

#include "client_rpc.h"
#include <time.h>
#include <stdio.h>

#define BILLION (long long int)1000000000

#define MAX_SIZE 4096

int StoreRPCClient::SayRead(int in, string& val)
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
    printf("%lld\n", diff); //<< std::endl;
    fflush( stdout );

    // Act upon its status.
    if (status.ok())
    {
        if (reply.errcode() == 0)
        {
            //memcpy(val.data(), reply.data().data(), MAX_SIZE);
            val = reply.data();
            //hsagar : should I resize? val.resize(MAX_SIZE);
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

int StoreRPCClient::SayGetLog(int in, WriteRequest &obj)
{
    LogRequest req;
    req.set_offset(in);
    LogResponse reply;
    ClientContext context;

    Status status = stub_->SayGetLog(&context, req, &reply);
    if (status.ok())
    {
        if (reply.retcode() == 1) // Done
        {
            return 1;
        }
        else // 0 means continue;
        {
            obj = reply.entry();
            return 0;
        }
    }
    return -1;
};

int StoreRPCClient::SayInternalReq(OP op , int in, string& val)
{
    if(op == OP_READ)
        return SayRead(in , val);
    else if(op == OP_WRITE)
        return SayWrite(in , val);
    return -1;
}


int StoreRPCClient::SayWrite(int in, string& val)
{
    struct timespec start, end, mid;
    long long int diff;

    WriteRequest req;
    req.set_address(in);
    if(val.size() > MAX_SIZE)
        val.resize(MAX_SIZE);
    req.set_data(val.data());

    WriteResponse reply;

    ClientContext context;

    clock_gettime(CLOCK_MONOTONIC, &start);
    Status status = stub_->SayWrite(&context, req, &reply);
    clock_gettime(CLOCK_MONOTONIC, &end);

    diff = BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
    printf("%lld\n", diff); //<< std::endl;
    fflush( stdout );
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
int StoreRPCClient::PingLeader()
{
    PingRequest req;
    PongResponse reply;

    req.set_request("Hello Leader, give response!");

    ClientContext context;
    Status status = stub_->HeartBeat(&context, req, &reply);
    if (status.ok())
    {
        if (reply.leader())
        {
            //cout << "Successful ping to leader" << endl;
            return 0;
        }
    }
    return -1;
}

int StoreRPCClient::PingBackup()
{
    PingRequest req;
    PongResponse reply;

    req.set_request("Hello Backup, give response!");

    ClientContext context;
    Status status = stub_->HeartBeat(&context, req, &reply);
    if (status.ok())
    {
        if (reply.leader() == false)
        {
            // cout << "Successful ping to backup" << endl;
            return 0;
        }
    }
    return -1;
}

bool flag = false;

void Client::SwitchServer()
{
    flag = !flag;
    if(flag)
        Initialize(BACKUP_IP , PRIMARY_IP);
    else
        Initialize(PRIMARY_IP , BACKUP_IP);

    while(primary_server->PingLeader() != 0)
    {
        sleep(5);
        cout << "Checking PingLeader" <<  primary_server->PingLeader() << endl;
        SwitchServer();
    }
}
void Client::Initialize(std::string pri_str , std::string sec_str)
{
    //cout << "Switch p to " << pri_str << " b to" << sec_str  << endl;
    pri_str = pri_str + ":" + mPort;
    sec_str = sec_str + ":" + mPort;
    grpc::ChannelArguments ch_args;
    ch_args.SetMaxReceiveMessageSize(INT_MAX);
    ch_args.SetMaxSendMessageSize(INT_MAX);
    if(primary_server == NULL || primary_server->PingLeader() != 0)
    {
        primary_server = new StoreRPCClient(
                grpc::CreateCustomChannel(pri_str, grpc::InsecureChannelCredentials(), ch_args), pri_str);
        back_server = new StoreRPCClient(
                    grpc::CreateCustomChannel(sec_str, grpc::InsecureChannelCredentials(), ch_args), sec_str);
    }
}

int Client::SayReq(OP op , int in, string& val)
{
    int result = primary_server->SayInternalReq(op, in , val);
    int retry = 1;
    while (result != 0 && retry < MAX_RETRY)
    {
        result = primary_server->SayInternalReq(op, in , val);
        retry = retry + 1;
    }
    if (retry == MAX_RETRY)
    {
        std::string ping_cmd = "ping -c1 -s1 " + primary_server->mIP + " > /dev/null 2>&1";
        int server_check = std::system(ping_cmd.data());
        if (server_check != 0)
        {
            SwitchServer();
            if( primary_server->SayInternalReq(op, in , val) != 0 )
            {
                cout << "Both Sever Down" << endl;
                return -1;
            }
        }
    }
    return 0;
}
int StoreRPCClient::Ping(PongResponse *reply)
{
    PingRequest req;

    req.set_request("Hello, give response!");

    ClientContext context;
    Status status = stub_->HeartBeat(&context, req, reply);
    if (status.ok())
    {
        return 0;
    }
    return -1;

}
int StoreRPCClient::SetCrashpointClient(int n)
{
    CrashRequest request;
    CrashResponse reply;

    request.set_request(n);

    ClientContext context;
    Status status = stub_->SetCrashpoint(&context, request, &reply);
    if (status.ok())
    {
        return reply.reply();
    }
    return -1;

}
