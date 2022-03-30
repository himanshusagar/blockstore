#include <signal.h>
#include "server_rpc.h"
#include "client_rpc.h"

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

void run_server()
{
    string hostbuffer;
    string backup_str;
    string server_address;
    hostbuffer.resize(256);

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

    StoreRPCServiceImpl service(backup_str);
    if (hostbuffer[4] == '0')
    {
        service.leader = true;
    }
    else
    {
        service.leader = false;
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
