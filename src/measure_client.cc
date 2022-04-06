#include <signal.h>
#include "client_rpc.h"
#include <random>
#include <cstdlib>
#include <iostream>

#define MAX_SIZE 4096
#define MAX_VAL 100000000007

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

int main(int argc, char *argv[])
{
    // "ctrl-C handler"
    signal(SIGINT, sigintHandler);
    std::string port = argv[1];
    std::string action = argv[2];
    std::string action_type = argv[3];
    int count = atoi(argv[4]);

    char read_data[MAX_SIZE];
    std::string write_data = "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";

    signal(SIGINT, sigintHandler);
    const std::string target_str = "10.10.1.1:" + port;
    grpc::ChannelArguments ch_args;

    ch_args.SetMaxReceiveMessageSize(INT_MAX);
    ch_args.SetMaxSendMessageSize(INT_MAX);

    StoreRPCClient storeRpc(
        grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials(), ch_args));

    if (action_type == "random")
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(0, MAX_VAL - 4200);

        for (int i = 0; i < count; i++)
        {
            if (action == "read")
            {
                storeRpc.SayRead(dis(gen), read_data);
            }
            else
            {
                storeRpc.SayWrite(dis(gen), write_data.data());
            }
        }
    }
    else
    {
        std::int64_t diff = MAX_VAL / count;
        for (std::int64_t i = 0; i < count; i += diff)
        {
            if (action == "read")
            {
                storeRpc.SayRead(i, read_data);
            }
            else
            {
                storeRpc.SayWrite(i, write_data.data());
            }
        }
    }

    return 0;
}
