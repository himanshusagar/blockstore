//
// Created by Himanshu Sagar on 3/4/22.
//

#ifndef BLOCKSTORE_PROFILE_H
#define BLOCKSTORE_PROFILE_H

#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <numeric>
#include <functional>

class TimeLog
{
    std::vector<uint64_t> values;
    std::string name;
public:
    TimeLog(): name("EMPTY"){}
    TimeLog(const std::string name_): name("client_" + std::move(name_)) {}
    void add(uint64_t ns) {
        values.push_back(ns);
    }
    ~TimeLog() {
        std::cout << name << ", ";
        if (values.size() == 1)
        {
            //Only one value
            std::cout << values.front() << "\n";
        }
        else
        {
            // Averaging.
            const auto sum = std::accumulate(values.begin(), values.end(), 0ULL);
            double dSize = values.size();
            std::cout << (int) ( ( (double) sum) / dSize ) << "\n";
        }
    }
};

struct UnitEntry
{
    TimeLog& mStats;
    std::chrono::high_resolution_clock::time_point begin;
    UnitEntry(TimeLog& m) :
            mStats(m), begin(std::chrono::high_resolution_clock::now()) { }
    ~UnitEntry()
    {
        auto d = std::chrono::high_resolution_clock::now() - begin;
        uint64_t countValue = std::chrono::duration_cast<std::chrono::microseconds>(d).count();
        mStats.add(countValue);
    }
};
/*
int main()
{
    {
        TimeLog timeLog("loop_time");
        for(int i = 0 ; i < 10 ; i++)
        {
            UnitEntry p(timeLog);
            int n = 0;
            while(n < 1e9)
                n++;
        }
    }
    return 0;
}
*/
#endif //BLOCKSTORE_PROFILE_H
