#include <signal.h>
#include "client_rpc.h"
#include <benchmark/benchmark.h>
#include "local_file.h"


void sigintHandler(int sig_num) {
    std::cerr << "Clean Shutdown\n";
    //    if (srv_ptr) {
    //        delete srv_ptr;
    //    }
    fflush(stdout);
    std::exit(0);
}


int main(int argc, char *argv[]) {
    // "ctrl-C handler"
    signal(SIGINT, sigintHandler);
    std::string port = argv[1];
    cout << "On Port " << port << endl;
    Client clientObj(port);
    LocalFile localFile;

    cout << "R/W Offset <Data> " << endl;


    int offset, limit;
    std::string dataVal;
    int retCode = 0;

    for (int i = 0; i < 10; i++) {
        dataVal = to_string(i);
        retCode = clientObj.SayReq(OP_WRITE, i, dataVal);
        if (retCode != 0)
            cout << "Write at" << i << " failed";
        localFile.Write(i, dataVal);

        std::string readRemote, readLocal;
        retCode = clientObj.SayReq(OP_READ, i, readRemote);
        if (retCode != 0)
            cout << "Read at" << i << " failed";
        localFile.Read(i, readLocal);
        if (localFile.checkSumFailed(readLocal, readRemote)) {
            cout << "Check Sum Failed at" << endl;
        }
    }

    return 0;
}
