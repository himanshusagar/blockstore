#include <signal.h>
#include "client_rpc.h"

#define MAX_SIZE 4096

void sigintHandler(int sig_num)
{
    std::cerr << "Clean Shutdown\n";
    //    if (srv_ptr) {
    //        delete srv_ptr;
    //    }
    fflush(stdout);
    std::exit(0);
}

int main(int argc, char *argv[])
{
    // "ctrl-C handler"
    signal(SIGINT, sigintHandler);
    const std::string target_str = "10.10.1.1:50051";
    grpc::ChannelArguments ch_args;

    ch_args.SetMaxReceiveMessageSize(INT_MAX);
    ch_args.SetMaxSendMessageSize(INT_MAX);

    StoreRPCClient storeRpc(
        grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials(), ch_args));

    char data[MAX_SIZE];
    std::string name = "kaushik";
    std::cout << storeRpc.SayWrite(92, name.data()) << std::endl;
    std::cout << storeRpc.SayRead(92, data) << std::endl;
    std::cout << data[0] << std::endl;
    std::cout << storeRpc.PingLeader() << std::endl;
    std::cout << storeRpc.PingBackup() << std::endl;
    
    return 0;
}
