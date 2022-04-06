#include <signal.h>
#include "client_rpc.h"
#include <random>
#include <cstdlib>
#include <iostream>
#include <thread>

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

int workload_exec(std::string port, std::string action, std::string action_type, int count)
{

    std::string write_data(4096, 'k');
    std::string read_data;
    read_data.resize(MAX_SIZE , '0');
    std::string target_str = "10.10.1.1:" + port;
    grpc::ChannelArguments ch_args;

    ch_args.SetMaxReceiveMessageSize(INT_MAX);
    ch_args.SetMaxSendMessageSize(INT_MAX);
    StoreRPCClient storeRpc(
        grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials(), ch_args) , target_str);

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
                storeRpc.SayWrite(dis(gen), write_data);
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
                storeRpc.SayWrite(i, write_data);
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    // "ctrl-C handler"
    signal(SIGINT, sigintHandler);
    std::string port = argv[1];
    std::string action = argv[2];
    std::string action_type = argv[3];
    int count = atoi(argv[4]);
    int thread_count = atoi(argv[5]);
    signal(SIGINT, sigintHandler);

    std::thread t[thread_count];

    for (int i = 0; i < thread_count; i++)
    {
        t[i] = std::thread(workload_exec, port, action, action_type, count);
    }

    for (int i = 0; i < thread_count; i++)
    {
        t[i].join();
    }
    return 0;
}
