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
    std::string action = argv[1];
    std::string action_type = argv[2];
    int count = atoi(argv[3]);

    cout << count << endl;
    char read_data[MAX_SIZE];
    std::string write_data = "kaushik";

    signal(SIGINT, sigintHandler);
    const std::string target_str = "10.10.1.1:50051";
    grpc::ChannelArguments ch_args;

    ch_args.SetMaxReceiveMessageSize(INT_MAX);
    ch_args.SetMaxSendMessageSize(INT_MAX);

    StoreRPCClient storeRpc(
        grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials(), ch_args));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0, MAX_VAL - 50);

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
    return 0;
}
