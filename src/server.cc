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

     service->connOtherServer = new StoreRPCClient(
        grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials(), ch_args));

    if(!service->leader)
        service->PerformRecovery();

    while(true)
    {
        int ret;
        usleep(500);
        // cout<< "heartbeat thread: "<<service->failed_heartbeats <<endl;
        if (leader){
            ret =  service->connOtherServer->PingBackup();
        }
        else{
            ret =  service->connOtherServer->PingLeader();
        }
        if (ret !=0){
            service->failed_heartbeats += 1;
            cout<< "Failed heartbeat " << service->failed_heartbeats <<endl;
        }
        else{
            service->failed_heartbeats = 0;
        }
        if (service->failed_heartbeats > service->retries)
        {
            cout << "Time to switch mode " << pthread_self() <<endl;
            // switch logic
            if (leader){
                // backup died. write request will add to queue
                service->backupIsActive = false;
            }
            if (leader != true){
                // leader died. need to change mode to leader
                service->leader = true;
                service->backupIsActive = false;
            }
             int value; 
            // waiting till backup/leader comes back alive
            sem_getvalue(&(service->mutex), &value); 
            cout<< "value: before" << value<< endl;
            sem_wait(&(service->mutex)); // 1 -> 0
            sem_wait(&(service->mutex)); // 0 -> -1
           
            sem_getvalue(&(service->mutex), &value); 
            cout<< "value: after" << value<< endl;
            service->failed_heartbeats = 0;
            service->connOtherServer = new StoreRPCClient(grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials(), ch_args));

            // wait over ?
            // TODO: Add logic for recovery
            continue;
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
    cout << getpid() << endl;
    sem_init(&service.mutex, 0, 1);
    thread t1(heartbeat_thread, service.leader, backup_str, &service);


    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char *argv[])
{
    // "ctrl-C handler"
    signal(SIGINT, sigintHandler);
    CrashPoints::g_spnt = S_NO_CRASH;
    if(argc >= 2)
    {
        int crashP = std::stoi(argv[1]);
        if( (S_POINTS)crashP < S_MAX)
            CrashPoints::g_spnt =  (S_POINTS)crashP;
        else
            std::cout << "Server Unable to set crash point" << endl;
    }
    run_server();
}
