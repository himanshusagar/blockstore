#include <signal.h>
#include "client_rpc.h"
#include <benchmark/benchmark.h>

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

int run_client()
{
    // "ctrl-C handler"
    signal(SIGINT, sigintHandler);
    std::string port = "5555";

    std::string pri_str = "10.10.1.1:" + port;
    std::string sec_str = "10.10.1.2:" + port;
    std::string curr_str = pri_str;

    int max_retry = 3;
    int retry = 0;

    grpc::ChannelArguments ch_args;

    ch_args.SetMaxReceiveMessageSize(INT_MAX);
    ch_args.SetMaxSendMessageSize(INT_MAX);

    StoreRPCClient storeRpc1(
            grpc::CreateCustomChannel(pri_str, grpc::InsecureChannelCredentials(), ch_args));

    StoreRPCClient storeRpc2(
            grpc::CreateCustomChannel(sec_str, grpc::InsecureChannelCredentials(), ch_args));

    StoreRPCClient *active_server;
    active_server = &storeRpc1;
    char data[MAX_SIZE];
    std::string name = "kaushik";

    int result = active_server->SayWrite(92, name.data());
    while (result != 0 && retry < max_retry)
    {
        result = active_server->SayWrite(92, name.data());
        retry = retry + 1;
        cout << "Retrying" << endl;
    }
    if (retry == max_retry)
    {
        std::string ping_cmd = "ping -c1 -s1 " + curr_str + " > /dev/null 2>&1";
        int server_check = std::system(ping_cmd.data());
        if (server_check != 0)
        {
            cout << "Changing the Primary Server" << endl;
            if (active_server == &storeRpc1)
            {
                active_server = &storeRpc2;
                curr_str = sec_str;
            }
            else
            {
                active_server = &storeRpc1;
                curr_str = pri_str;
            }
        }
    }

    string some1 = "dummyData" , some;
    some.resize(MAX_SIZE , '.');
    std::cout << "written : " << active_server->SayWrite(20, some1.data()) << std::endl;
    std::cout << "read : " << active_server->SayRead(20, some.data()) << std::endl;
    std::cout << some << std::endl;
    //std::cout << active_server->PingLeader() << std::endl;
    //std::cout << active_server->PingBackup() << std::endl;

}

static void BM_SomeFunction(benchmark::State& state) {
    // Perform setup here
    for (auto _ : state) {
        // This code gets timed
        run_client();
    }
}

//int main(int argc, char *argv[])
//{
//    g_argc = argc;
//    g_argv = argv;
//
//
//
//
//    //run_client(argc, argv);
//    return 0;
//}

BENCHMARK(BM_SomeFunction);
BENCHMARK_MAIN();