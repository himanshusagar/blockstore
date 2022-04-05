//
// Created by Himanshu Sagar on 3/26/22.
//

#include "server_rpc.h"
#include <unistd.h>

#define MAX_SIZE 4096

using grpc::Status;

int StoreRPCServiceImpl::PerformRecovery()
{
    WriteRequest entry;
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
            cout << "Write Failed" << endl;
            break;
        }
        index++;
    }
    cout << "Recovery Done" << endl;
}
Status StoreRPCServiceImpl::SayRead(ServerContext *context, const ReadRequest *request, ReadResponse *response)
{
    int address = request->address();
    char buff[MAX_SIZE];
    lseek(storefd, address, SEEK_SET);
    int result = read(storefd, buff, MAX_SIZE);
    response->set_data(buff);
    if (result == -1)
    {
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
    if(leader)
        CrashPoints::serverCrash(PRIMARY_AFTER_WRITE_REQ_RECV);
    else
        CrashPoints::serverCrash(BACKUP_AFTER_WRITE_REQ_RECV);

    int address = request->address();
    int retry = 0;
    Request *requestNode = NULL;
    requestNode = new Request();
    int rep_result = -1;
    requestNode->address = address;
    requestNode->data = request->data().data();
    lseek(storefd, address, SEEK_SET);
    if (leader)
    {
        // request_queue.push_front(requestNode);
    }
    requestMap[address] = requestNode;
    // Main Action
    cout << "Write" << endl;
    int result = write(storefd, request->data().data(), MAX_SIZE);
    if (result == -1)
    {
        cout << "Write Failed" << endl;
        response->set_errcode(errno);
    }

    if(leader)
        CrashPoints::serverCrash(PRIMARY_AFTER_WRITE);
    else
        CrashPoints::serverCrash(BACKUP_AFTER_WRITE);

    // Checking if the current instance is primary
    if (leader && backupIsActive)
    {
        while (retry < maxRetry && rep_result != 0)
        {
            rep_result = storeReplicateRpc->SayWrite(address, request->data().data());
            cout << "Replicate Result Status" << rep_result << endl;
            retry = retry + 1;
            if (rep_result != 0)
            {
                cout << rep_result << endl;
                response->set_errcode(rep_result);

                cout << "Replication on Backup failed and will be retried" << endl;
            }
            else
            {
                CrashPoints::serverCrash(PRIMARY_AFTER_ACK_FROM_B);
                // request_queue.pop_back();
                requestMap.erase(address);
                cout << "Replication on Backup is successfull" << endl;
            }
            if (rep_result != 0)
            {
                cout << "Replication on Backup is failed after several retries" << endl;
                cout << "Making Backup Inactive" << endl;
                request_queue.push_front(request);
                backupIsActive = false;
            }
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
        const WriteRequest *obj = request_queue.at(index);
        response->set_allocated_entry(const_cast<WriteRequest *>(obj));
        response->set_retcode(0);
        cout << "Inside SayGetLog" << obj->address() << " " << obj->data() << endl;
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
    // cout << "HeartBeat: " << req << endl;
    response->set_response("I am alive!");
    response->set_leader(leader);

    // cout << "value "<< value <<endl;
    int value, ret;
    sem_getvalue(&mutex, &value);
    while (value == 0)
    {
        cout << "HeartBeat value: before" << value << endl;
        ret = sem_post(&mutex);
        sem_getvalue(&mutex, &value);
        cout << "HeartBeat value: after" << value << endl;
    }
    return Status::OK;
}
