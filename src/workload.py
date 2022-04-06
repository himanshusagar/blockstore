#!/usr/bin/env python3

from datetime import datetime
import random
import sys
import os
import subprocess
import argparse
import threading

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
        self.action, self.action_type, self.count, self.clients = "write", "random", 100, 100
        print(f"Simulating the workload action={self.action} type={self.action_type} count={self.count} ..............")
        self.simulate_workload()
        print("Simulation Complete")

        # Workload 2
        self.action, self.action_type, self.count, self.clients = "write", "sequential", 100, 100
        print(f"Simulating the workload action={self.action} type={self.action_type} count={self.count} ..............")
        self.simulate_workload()
        print("Simulation Complete")

        # Workload 3
        self.action, self.action_type, self.count, self.clients = "read", "random", 100, 100
        print(f"Simulating the workload action={self.action} type={self.action_type} count={self.count} ..............")
        self.simulate_workload()
        print("Simulation Complete")

        # Workload 4
        self.action, self.action_type, self.count, self.clients = "read", "sequential", 100, 100
        print(f"Simulating the workload action={self.action} type={self.action_type} count={self.count} ..............")
        self.simulate_workload()
        print("Simulation Complete")

        # Workload 5
        self.action, self.action_type, self.count, self.clients = "mixed", "random", 100, 100
        print(f"Simulating the workload action={self.action} type={self.action_type} count={self.count} ..............")
        self.simulate_workload()
        print("Simulation Complete")


    def simulate_workload(self):
        output_file = "_".join([self.action, self.action_type, str(self.count), datetime.now().strftime("%d_%m_%Y_%H_%M_%S")])
        subprocess.call(['touch', output_file])
        process = subprocess.Popen("./measure_client " + str(self.port) + " " + self.action + " " +
                                   self.action_type + " " + str(self.count) + " " + str(self.clients) + " >> " + output_file, shell=True)
        out, err = process.communicate()
        errcode = process.returncode
        process.kill()
        process.terminate()
        self.parse_file(output_file)

    def parse_file(self, output_file):
        file = open(output_file, 'r')
        lines = file.readlines()
        tot_time = 0.0
        for line in lines:
            if line == '':
                self.count = self.count - 1
                continue
            tot_time += float(line)
        self.average = tot_time / self.count
        print(f"Results: average={self.average}")            


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    wl = Workload()
    wl.simulate_workloads()