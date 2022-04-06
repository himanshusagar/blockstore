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
        self.simulate_workload()

    def simulate_workload(self):
        output_file = "_".join([self.action, self.action_type, str(self.count), datetime.now().strftime("%d_%m_%Y_%H_%M_%S")])
        subprocess.call(['touch', output_file])
        process = subprocess.Popen("./measure_client " + str(self.port) + " " + self.action + " " +
                                   self.action_type + " " + str(self.count) + " >> " + output_file, shell=True)
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
                self.count = self.cout - 1
                continue
            if len(line) > 10:
                self.count = self.cout - 1
                continue
            tot_time += float(line)
        self.average = tot_time / self.count
        print(f"action={self.action} type={self.action_type} count={self.count} average={self.average}")            


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    wl = Workload()
    wl.simulate_workloads()