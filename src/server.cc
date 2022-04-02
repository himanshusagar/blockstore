#include <signal.h>
#include "server_rpc.h"
#include "client_rpc.h"
#include <thread>
#include <unistd.h>

using namespace std;

void sigintHandler(int sig_num)
{
    std::cerr << "Clean Shutdown\n";
    //    if (srv_ptr) {
    //        delete srv_ptr;
    //    }
    fflush(stdout);
    std::exit(0);
}

void heartbeat_thread(bool leader, string address, StoreRPCServiceImpl *service){

    const std::string target_str = address;
    grpc::ChannelArguments ch_args;

    ch_args.SetMaxReceiveMessageSize(INT_MAX);
    ch_args.SetMaxSendMessageSize(INT_MAX);

    StoreRPCClient storeRpc(
        grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials(), ch_args));

    while(true)
    {
        int ret;
        usleep(100);
        if (service->failed_heartbeats > service->retries){
            cout << "Time to switch mode" <<endl;
            // TODO add switch logic
            continue;
        }

        if (leader){
            ret = storeRpc.PingBackup();
        }
        else{
            ret = storeRpc.PingLeader();
        }
        if (ret !=0){
            service->failed_heartbeats += 1;
        }
        else{
            service->failed_heartbeats = 0;
        }
    }
}

void run_server()
{
    string hostbuffer;
    string backup_str;
    string phase;
    string server_address;
    hostbuffer.resize(256);
    phase = "start";
    int hostname = gethostname(hostbuffer.data(), hostbuffer.size());
    if (hostbuffer[4] == '0')
    {
        backup_str = "10.10.1.2:50051";
        server_address = "10.10.1.1:50051";
    }
    else
    {
        backup_str = "10.10.1.1:50051";
        server_address = "10.10.1.2:50051";
    }
    cout << "curr host is " << hostbuffer << endl;
    cout << "backup host is " << backup_str << endl;

    StoreRPCServiceImpl service(backup_str, phase);
    service.failed_heartbeats = 0;
    if (hostbuffer[4] == '0')
    {
        service.leader = true;
        service.backupIsActive = true;
    }
    else
    {
        service.leader = false;
        service.backupIsActive = false;
    }    

    std::cout << hostbuffer << "  " << service.leader << std::endl;
    //  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.SetMaxSendMessageSize(INT_MAX);
    builder.SetMaxReceiveMessageSize(INT_MAX);
    builder.SetMaxMessageSize(INT_MAX);

    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    thread t1(heartbeat_thread, service.leader, backup_str, &service);

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main()
{
    // "ctrl-C handler"
    signal(SIGINT, sigintHandler);
    run_server();
}
