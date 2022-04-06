//
// Created by Himanshu Sagar on 4/6/22.
//

#ifndef BLOCKSTORE_LOCAL_FILE_H
#define BLOCKSTORE_LOCAL_FILE_H
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#define LOCAL_MAX_SIZE 4096
#define MAX_LOOP_ITER 10

using namespace std;
const static string sPathName = "localFile.txt";
typedef long long int ll;

class LocalFile
{

    int localFD;
    std::hash<std::string> mStrHash;
public:
    LocalFile()
    {
        localFD = open(sPathName.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
        std::string someData(LOCAL_MAX_SIZE , '0');
        for (int i = 0; i < MAX_LOOP_ITER ; ++i) {
            write(localFD , someData.data() , LOCAL_MAX_SIZE);
        }
    }
    int Write(ll address, std::string& val)
    {
        lseek(localFD, address, SEEK_SET);
        int result = write(localFD, val.data(), LOCAL_MAX_SIZE);
        if (result == -1)
        {
            cout << "local write failed" << endl;
            return -1;
        }
        return 0;
    }
    int Read(ll address, std::string& val)
    {
        lseek(localFD, address, SEEK_SET);
        int result = read(localFD, val.data(), LOCAL_MAX_SIZE);
        if (result == -1)
        {
            cout << "local read Failed" << endl;
        }
        return 0;
    }

    bool checkSumFailed(std::string& src, std::string& dest)
    {
        return (mStrHash(src)!=mStrHash(dest));
    }

};


#endif //BLOCKSTORE_LOCAL_FILE_H
