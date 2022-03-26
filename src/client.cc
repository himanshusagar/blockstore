#include <signal.h>
#include "client_rpc.h"

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
    const std::string target_str = "localhost:50051";
    grpc::ChannelArguments ch_args;

    ch_args.SetMaxReceiveMessageSize(INT_MAX);
    ch_args.SetMaxSendMessageSize(INT_MAX);

    StoreRPCClient storeRpc(
            grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials() , ch_args ));

    std::cout << storeRpc.SayRead(92) << std::endl;
    return 0;
}
