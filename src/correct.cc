#include <signal.h>
#include "client_rpc.h"
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
    int offset, limit;
    std::string writeData;
    int retCode = 0;

    for (int i = 0; i < MAX_LOOP_ITER; i++) {

        writeData = std::string( 'X' , i + 1) ;
        retCode = clientObj.SayReq(OP_WRITE, i, writeData);
        if (retCode != 0)
            cout << "Write at" << i << " failed";
        localFile.Write(i, writeData);

        std::string readRemote, readLocal;
        retCode = clientObj.SayReq(OP_READ, i, readRemote);
        if (retCode != 0)
            cout << "Read at" << i << " failed";
        localFile.Read(i, readLocal);

        readLocal.resize(writeData.size());
        readRemote.resize(writeData.size());

        if (localFile.checkSumFailed(readLocal, readRemote)) {
            cout << "Check Sum Failed at " << i << endl;
        }
    }

    return 0;
}
