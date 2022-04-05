//
// Created by Himanshu Sagar on 4/4/22.
//

#ifndef BLOCKSTORE_CRASH_POINTS_H
#define BLOCKSTORE_CRASH_POINTS_H

#include <iostream>

enum C_POINTS {
    C_NO_CRASH = 0,
    C_AFTER_R_REQ = 1,
    C_AFTER_W_REQ = 2,
    C_MAX
};
enum S_POINTS
{
    S_NO_CRASH = 0,
    PRIMARY_AFTER_WRITE_REQ_RECV = 1,
    PRIMARY_AFTER_WRITE = 2,
    BACKUP_AFTER_WRITE_REQ_RECV = 3,
    BACKUP_AFTER_WRITE = 4,
    PRIMARY_AFTER_ACK_FROM_B = 5,
    PRIMARY_AFTER_ACK_TO_C = 6,
    S_MAX
};

class CrashPoints
{
public:
    static S_POINTS g_spnt;
    static C_POINTS g_cpnt;

    static void serverCrash(S_POINTS pnt)
    {
        if(pnt == g_spnt)
            memset(nullptr, 'X' , 10); //Crashing
    }
    static void clientCrash(C_POINTS pnt)
    {
        if(pnt == g_cpnt)
            memset(nullptr, 'X' , 10); //Crashing
    }

private:
    CrashPoints() = default;
};
S_POINTS CrashPoints::g_spnt = S_NO_CRASH;
C_POINTS CrashPoints::g_cpnt = C_NO_CRASH;
#endif //BLOCKSTORE_CRASH_POINTS_H
