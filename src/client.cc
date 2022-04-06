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

int main(int argc, char *argv[])
{
    // "ctrl-C handler"
    signal(SIGINT, sigintHandler);
    std::string port = argv[1];
    cout << "On Port " << port << endl;
    Client clientObj(port);
    cout << "R/W Offset <Data> " << endl;

    while(true)
    {
        char op;
        int offset;
        std::string dataVal;
       
        cout << ">>" ;
        cin>> op;
        cin>> offset;
        if(op == 'W')
        {
            cin >> dataVal;
            clientObj.SayReq( OP_WRITE , offset, dataVal);
            dataVal.clear();
        }
        else if( op == 'R')
        {
            dataVal.resize(64);
            clientObj.SayReq( OP_READ , offset, dataVal);
            cout << "read : "  << dataVal << endl;
        }
        else
            break;
    }
    return 0;
}

//static void BM_SomeFunction(benchmark::State& state) {
//    // Perform setup here
//    for (auto _ : state) {
//        // This code gets timed
//        run_client();
//    }
//}

//int main(int argc, char *argv[])
//{
//    return run_client(argc, argv);
//}

//BENCHMARK(BM_SomeFunction);
//BENCHMARK_MAIN();