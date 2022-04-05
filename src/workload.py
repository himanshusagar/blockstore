#!/usr/bin/env python3

from datetime import datetime
import random
import sys
import os
import subprocess

class Workload:
    def __init__(self, action="read", action_type="random", count=100, port=50051):
        self.action = action
        self.action_type = action_type
        self.count = count
        self.output_file = "_".join([action, action_type, count, datetime.now().strftime("%d/%m/%Y %H:%M:%S")])
        self.workloads = []
        self.port = port

    def simulate_work_loads(self):
        self.simulate_workloads()

    def simulate_workloads(self):
        process = subprocess.Popen("./measure_client" + " " + self.action + " " +
                                   self.action_type + " " + self.count + " >> " + self.output_file, shell=False)
        out, err = process.communicate()
        errcode = process.returncode
        print(errcode)
        process.kill()
        process.terminate()
        self.parse_file()

    def parse_file(self):
        file = open(self.output_file, 'r')
        lines = file.readlines()
        tot_time = 0.0
        for line in lines:
            tot_time += float(line)
        print(f"action={} type={} count={} average={}".format(self.action, self.action_type, self.count, self.average))



if __name__ == '__main__':
    ap = argparse.ArgumentParser()