//
// Created by Himanshu Sagar on 3/26/22.
//

#include "server_rpc.h"
#include <unistd.h>

#include <utility>

#define MAX_SIZE 4096

using grpc::Status;

int StoreRPCServiceImpl::PerformRecovery()
{
    LogEntry entry;
    int index = 0;

    while (connOtherServer->SayGetLog(index, entry) == 0)
    {
        cout << "Inside PerformRecovery " << index << " " << entry.address() << " " << entry.data() << endl;
        if (entry.address() == 0)
        {
            cout << "Unfilled entry" << endl;
            return -1;
        }
        lseek(storefd, entry.address(), SEEK_SET);
        int result = write(storefd, entry.data().data(), MAX_SIZE);
        if (result == -1)
        {
            cout << "Write Failed. Recovery stopped." << endl;
            return -1;
            // break;
        }
        index++;
    }
    //cout << "Recovery Done" << endl;
    return 0;
}
Status StoreRPCServiceImpl::SayRead(ServerContext *context, const ReadRequest *request, ReadResponse *response)
{
    int address = request->address();
    string buff;
    buff.resize(4096 , '0');
    lseek(storefd, address, SEEK_SET);
    int result = read(storefd, buff.data(), MAX_SIZE);
    response->set_data(buff);
    if (result == -1)
    {
        cout << "Read Failed" << endl;
        response->set_errcode(errno);
    }
    else
    {
        response->set_errcode(0);
    }
    return Status::OK;
}

Status StoreRPCServiceImpl::SayWrite(ServerContext *context, const WriteRequest *request, WriteResponse *response)
{
    if (leader)
        CrashPoints::serverCrash(PRIMARY_AFTER_WRITE_REQ_RECV);
    else
        CrashPoints::serverCrash(BACKUP_AFTER_WRITE_REQ_RECV);

    int address = request->address();
    int retry = 0;
    int rep_result = -1;
    lseek(storefd, address, SEEK_SET);
    // Main Action
    // cout << "Write" << endl;
    int result = write(storefd, request->data().data(), MAX_SIZE);
    if (result == -1)
    {
        cout << "Write Failed" << endl;
        response->set_errcode(errno);
        return Status::OK;
    }
    
    if (leader)
        CrashPoints::serverCrash(PRIMARY_AFTER_WRITE);
    else
        CrashPoints::serverCrash(BACKUP_AFTER_WRITE);

    // Checking if the current instance is primary
    if (leader && replication)
    {
      //  while (retry < maxRetry && rep_result != 0)
        {
            std::string val = request->data();
            rep_result = connOtherServer->SayWrite(address, val);
            // cout << "Replicate Result Status" << rep_result << endl;
            retry = retry + 1;
            // if (rep_result != 0)
            // {
            //     cout << rep_result << endl;
            //     response->set_errcode(rep_result);
            //     cout << "Replication on Backup failed and will be retried" << endl;
            //     continue;
            // }
            //else
            //{
                CrashPoints::serverCrash(PRIMARY_AFTER_ACK_FROM_B);
                // request_queue.pop_back();
                // cout << "Replication on Backup is successfull" << endl;
           // }
        }
        if (rep_result != 0)
        {
            cout << "Replication on Backup is failed after several retries" << endl;
            cout << "Making Backup Inactive" << endl;
            LogEntry entry(request->address() , request->data() );
            request_queue.push_back(entry);
            backupIsActive = false;
        }
        // Sending to the primary backup
    }
    response->set_errcode(0);

    return Status::OK;
}

Status StoreRPCServiceImpl::SayGetLog(ServerContext *context, const LogRequest *request, LogResponse *response)
{
    long index = request->offset();
    if (0 <= index && index < request_queue.size())
    {
        // In range
        const LogEntry entry = request_queue.at(index);
        response->set_address(entry.address());
        response->set_data(entry.data());
        response->set_retcode(0);
        cout << "Inside SayGetLog" << entry.address() << " " << entry.data() << endl;
    }
    else
    {
        // Done
        request_queue.clear();
        backupIsActive = true;
        response->set_retcode(1);
    }

    return Status::OK;
}

Status StoreRPCServiceImpl::HeartBeat(ServerContext *context, const PingRequest *request, PongResponse *response)
{
    string req = request->request();
    //cout << "HeartBeat: " << req << endl;
    response->set_response("I am alive!");
    response->set_leader(leader);

    pthread_mutex_lock(&mp);
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mp);
    return Status::OK;
}

Status StoreRPCServiceImpl::SetCrashpoint(ServerContext *context, const CrashRequest *request, CrashResponse *response)
{
    int req = request->request();
    if (req < S_MAX){
        CrashPoints::g_spnt = (S_POINTS)req;
        response->set_reply(0);
    }
    else{
        response->set_reply(-1);
    }
    return Status::OK;
}

long long int LogEntry::address() const {
    return mAddress;
}

void LogEntry::setAddress(long long int address) {
    mAddress = address;
}

const string &LogEntry::data() const {
    return mData;
}

void LogEntry::setData(const string &data) {
    mData = data;
}

LogEntry::LogEntry(long long int address, string data) : mAddress(address), mData(std::move(data)) {}
