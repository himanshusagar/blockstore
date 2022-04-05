#!/usr/bin/env python

from datetime import datetime
import random
import sys
import os
import subprocess

class Workload:
    def __init__(self, action="read", action_type="random", count=100):
        self.action = action
        self.action_type = action_type
        self.count = count
        self.output_file = "_".join([action, action_type, count, datetime.now().strftime("%d/%m/%Y %H:%M:%S")])
        self.workloads = []

    def simulate_work_loads(self):
        if self.type == "random":
            self.simulate_random_workloads()
        if self.type == "sequential":
            self.simulate_sequential_loads()

    def simulate_random_workloads(self):
        process = subprocess.Popen("./client" + " " + action + " " +
                                   action_type + " " + count + " >> " + self.output_file, shell=False)
        out, err = process.communicate()
        errcode = process.returncode
        print(errcode)
        process.kill()
        process.terminate()

    def parse_file(self):
        file = open(self.output_file, 'r')
        lines = file.readlines()
        tot_time = 0.0
        for line in lines:
            tot_time += float(line)
        print(f"action={} type={} count={} average={}".format(self.action, self.action_type, self.count, self.average))



if __name__ == '__main__':
    ap = argparse.ArgumentParser()