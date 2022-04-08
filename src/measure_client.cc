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

int workload_consistency(std::string port)
{
    std::string write_data_1(4096, 'k');
    std::string write_data_2(4096, 'a');
    std::string read_data;
    read_data.resize(MAX_SIZE, '0');
    std::string target_str = "10.10.1.1:" + port;

    grpc::ChannelArguments ch_args;

    ch_args.SetMaxReceiveMessageSize(INT_MAX);
    ch_args.SetMaxSendMessageSize(INT_MAX);
    StoreRPCClient storeRpc(
        grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials(), ch_args), target_str);

    storeRpc.SayWrite(10, write_data_1);
    cout << "Write at address 10 was successfull" << endl;
    storeRpc.SayRead(10, read_data);
    cout << "Read the data at address 10 " << read_data << endl;
    storeRpc.SayWrite(15, write_data_2);
    cout << "Write at address 15 was successfull" << endl;
    storeRpc.SayRead(10, read_data);
    cout << "Read the data at address 15 " << read_data << endl;

    return 0;
}

int workload_perf(std::string port, std::string action, std::string action_type, int count)
{

    std::string write_data(4096, 'k');
    std::string read_data;
    read_data.resize(MAX_SIZE, '0');
    std::string target_str = "10.10.1.1:" + port;
    grpc::ChannelArguments ch_args;

    ch_args.SetMaxReceiveMessageSize(INT_MAX);
    ch_args.SetMaxSendMessageSize(INT_MAX);
    StoreRPCClient storeRpc(
        grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials(), ch_args), target_str);

    std::random_device mixed_rd;
    std::mt19937 mixed_gen(mixed_rd());
    std::uniform_real_distribution<float> mixed_dis(0, 1);

    std::string mixed_action = "none";
    if (action_type == "random")
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(0, MAX_VAL - 4200);

        for (int i = 0; i < count; i++)
        {
            if (action == "mixed")
            {
                if (mixed_dis(mixed_gen) <= 0.1)
                {
                    mixed_action = "read";
                }
                else
                {
                    mixed_action = "write";
                }
            }
            if (action == "read" || mixed_action == "read")
            {
                storeRpc.SayRead(dis(gen), read_data);
            }
            else
            {
                storeRpc.SayWrite(dis(gen), write_data);
            }
        }
    }
    else if (action_type == "sequential")
    {
        std::int64_t diff = MAX_VAL / count;
        for (int i = 1; i < count; i += 1)
        {
            if (action == "mixed")
            {
                if (mixed_dis(mixed_gen) <= 0.1)
                {
                    mixed_action = "read";
                }
                else
                {
                    mixed_action = "write";
                }
            }
            if (action == "read" || mixed_action == "read")
            {
                storeRpc.SayRead(i*diff, read_data);
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

    if (action == "consistency")
    {
        workload_consistency(port);
        return 0;
    }

    for (int i = 0; i < thread_count; i++)
    {
        t[i] = std::thread(workload_perf, port, action, action_type, count);
    }

    for (int i = 0; i < thread_count; i++)
    {
        t[i].join();
    }

    return 0;
}
