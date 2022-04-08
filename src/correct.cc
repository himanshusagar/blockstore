#include <signal.h>
#include "client_rpc.h"
#include "local_file.h"
#include <random>
#include "crash_points.h"

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

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(S_NO_CRASH , S_MAX);


    std::random_device mixed_rd;
    std::mt19937 mixed_gen(mixed_rd());
    std::uniform_real_distribution<float> mixed_dis(0, 1);
    std::string NULL_STRING;

    bool flag = true;
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


        if (flag && (mixed_dis(mixed_gen) <= 0.05) )
        {
            //Let's crash for 5% of requests.
            int randNum = dis(gen);
            //Randomly decide crash point
            clientObj.SayReq(OP_CRASH , randNum , NULL_STRING);
            if (retCode != 0)
                cout << "Crash Point at" << i << " " << randNum << " failed";
            flag = false;
        }

    }

    return 0;
}
