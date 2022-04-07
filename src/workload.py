#!/usr/bin/env python3

from datetime import datetime
import random
import sys
import os
import subprocess
import argparse
import threading
import numpy as np

class Workload:
    def __init__(self, action="write", action_type="random", count=100, port=50051, clients=100):
        self.action = action
        self.action_type = action_type
        self.count = count
        self.workloads = []
        self.port = port
        self.clients = clients

    def simulate_workloads(self):
        # Workload 1
        self.action, self.action_type, self.count, self.clients = "write", "random", 100, 1
        print(f"Simulating the workload action={self.action} type={self.action_type} count={self.count} ..............")
        self.simulate_workload()
        print("Simulation Complete")

        self.action, self.action_type, self.count, self.clients = "read", "random", 100, 1
        print(f"Simulating the workload action={self.action} type={self.action_type} count={self.count} ..............")
        self.simulate_workload()
        print("Simulation Complete")

    def simulate_workload(self):
        output_file = "_".join([self.action, self.action_type, str(self.count), datetime.now().strftime("%d_%m_%Y_%H_%M_%S")])
        subprocess.call(['touch', output_file])
        process = subprocess.Popen("./measure_client 14795 " + self.action + " " +  self.action_type + " " + str(self.count) + " " + str(self.clients) + " >> " + output_file, shell=True)
        out, err = process.communicate()
        errcode = process.returncode
        process.kill()
        process.terminate()
        self.parse_file(output_file)

    def parse_file(self, output_file):
        file = open(output_file, 'r')
        lines = file.readlines()
        times = []
        for line in lines:
            times.append(float(line.strip()))
        times = np.array(times)
        self.average = np.average(times)
        self.median = np.median(times)
        self.nine_nine = np.percentile(times, 99)
        self.nine_zero = np.percentile(times, 90)
        print(f"Results: average={self.average} median={self.median} 99={self.nine_nine} 90={self.nine_zero}")            


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    wl = Workload()
    wl.simulate_workloads()
