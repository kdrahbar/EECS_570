import pylab as plt
import operator
import math
import numpy as np
import scipy.stats as stats
from bisect import bisect_left
from scipy.stats import norm
import matplotlib.pyplot as pltt

def get_time(line):
    time = None
    for l in line:
        try:
            time = float(l)
        except ValueError:
            pass
    return time

class NetworkCharicterizer():

    def __init__(self, strace_input=None, ftrace_input=None):
        if strace_input:
            self.strace_input_file = strace_input
        if ftrace_input:
            self.ftrace_input_file = ftrace_input

    def parse_strace(self):
        with open(self.strace_input_file) as f:
            lines = f.readlines()
        
        self.strace_counts = {}
        for line in lines:
            # Hacky way to ignore garbage line in strace output
            if "---" not in line and "%" not in line:
                line = line.split()
                count, syscall = int(line[3]), line[-1]
                if syscall in self.strace_counts:
                    self.strace_counts[syscall] += count
                else:
                    self.strace_counts[syscall] = count

    def parse_ftrace(self):
        with open(self.ftrace_input_file) as f:
            lines = f.readlines()
        
        self.ftrace_time = {} # list of timestamps for each function
        self.ftrace_counts = {} # count for each timestamp
        funcs_without_timestamp = []
        
        for i, line in enumerate(lines):
            if " us " in line:
                # its the end of a function so we need to get the time stamp and match
                # it up
                if line.strip()[-1] == "}":
                    ending_function = funcs_without_timestamp.pop()
                    line = line.split()
                    
                    for l in line:
                        if l.isdigit():
                            time = l
                    

                    if ending_function in self.ftrace_time:
                        self.ftrace_time[ending_function].append(time)
                        self.ftrace_counts[ending_function] += 1
                    else:
                        self.ftrace_time[ending_function] = [time]
                        self.ftrace_counts[ending_function] = 1

                elif "*" in line:
                    print line
                    orig = line
                    line = line.replace("/", "")
                    line = line.replace("*", "")
                    line = line.split()

                    time = get_time(line)
                    function_name = line[-1] + "()"

                    if function_name in self.ftrace_time:
                        self.ftrace_time[function_name].append(time)
                        self.ftrace_counts[function_name] += 1
                    else:
                        self.ftrace_time[function_name] = [time]
                        self.ftrace_counts[function_name] = 1

                else:
                    line = line.replace(";", "")
                    line = line.split()
                    time = get_time(line)
                    function_name = line[-1]

                    if function_name in self.ftrace_time:
                        self.ftrace_time[function_name].append(time)
                        self.ftrace_counts[function_name] += 1
                    else:
                        self.ftrace_time[function_name] = [time]
                        self.ftrace_counts[function_name] = 1

            else:
                # it the beginning of a function call
                if "{" in line:
                    line = line.split()
                    function_name = line[-2]
                    funcs_without_timestamp.append(function_name)

        # just doing clean up for random stuff that gets parsed
        bad_data = []
        for func, timestamps in self.ftrace_time.iteritems():
            if "()" not in func:
                bad_data.append(func)
            else:
                timestamps = [float(x) for x in timestamps]
                self.ftrace_time[func] = sum(timestamps)

        for entry in bad_data:
            self.ftrace_time.pop(entry, None)
            self.ftrace_counts.pop(entry, None)

    # n is the number of columns
    def graph_strace(self, n):
        sorted_counts = sorted(self.strace_counts.items(), key=operator.itemgetter(1))[-n:]
        sorted_counts = sorted_counts[:n]
        position = range(1, n+1)
        total_times = [x[1] for x in sorted_counts]
        LABELS = [x[0] for x in sorted_counts]

        fig = plt.figure()
        plt.bar(position, total_times, align='center')
        plt.xticks(position, LABELS, fontsize=5)
        fig.suptitle('GCE Server Syscalls', fontsize=20)
        plt.xlabel('Syscall', fontsize=12)
        plt.ylabel('Time (us)', fontsize=12)
        plt.show()        

    def graph_ftrace(self, n):
        sorted_counts = sorted(self.ftrace_time.items(), key=operator.itemgetter(1))[-(n+1):]
        sorted_counts = sorted_counts[:n]
        position = range(1, n+1)
        total_times = [math.log(x[1]) for x in sorted_counts]
        # total_times = [x[1] for x in sorted_counts]
        print sum(total_times)
        LABELS = [x[0] for x in sorted_counts]

        fig = plt.figure()
        plt.bar(position, total_times, align='center')
        plt.xticks(position, LABELS, fontsize=3)
        fig.suptitle('GCE Server Time Spent in Kernel Calls', fontsize=20)
        plt.xlabel('Funciton Name', fontsize=12)
        plt.ylabel('Time (us)', fontsize=12)
        plt.show()


class discrete_cdf:
    def __init__(self, data):
        self._data = data # must be sorted
        self._data_len = float(len(data))

    def __call__(self, point):
        return (len(self._data[:bisect_left(self._data, point)]) / 
                self._data_len)

def distb_plot(input_file):

    h = []
    with open(input_file) as f:
        content = f.readlines()
    for line in content:
        line = line.split()
        h.append(float(line[1])/1000000)
    h.sort()

    hmean = np.mean(h)
    hstd = np.std(h)
    pdf = stats.norm.pdf(h, hmean, hstd)

    pdf_sum = sum(pdf)
    cdf = [ 1 - (pdf_sum-pdf[0])/pdf_sum ]
    
    for i,val in enumerate(pdf):
        if i > 0:
            cdf.append(1 - (pdf_sum - pdf[i])/pdf_sum + cdf[i-1] )

    cdf = [1- v for v in cdf]
    cdf = [0 if v < 0 else v for v in cdf]
    fig = plt.figure()
    plt.style.use('ggplot')

    fig.suptitle('Cumulative Distribution Function for Latency on GCE', fontsize=13)
    
    plt.ylabel('Cumulative Percent ', fontsize=12)
    plt.xlabel('Time (ms)', fontsize=12)
    
    plt.plot(h, cdf) # including h here is crucial
    plt.show()


def bandwidth_plot(input_file):

    x_vals = []
    y_vals = []
    with open(input_file) as f:
        content = f.readlines()
    for line in content:
        line = line.split()
        x_vals.append(int(line[0])/1000000)
        print line
        if "ns" in line:
            y_vals.append(int(x_vals[-1])/(float(line[1])/1000000000))
        else:
            y_vals.append(int(x_vals[-1]/(int(line[1]))))

    fig = plt.figure()
    plt.style.use('ggplot')

    fig.suptitle('Bandwidth vs. Filesize', fontsize=13)
    
    plt.ylabel('Bandwidth (Mbps) ', fontsize=12)
    plt.xlabel('File size Mb', fontsize=12)
    
    plt.plot(x_vals, y_vals) # including h here is crucial
    plt.show()

if __name__ == "__main__":
    # input_file = "../results/ftrace/CE_client.txt"
    input_file = "../results/ftrace/CE_server.txt"
    characterizer = NetworkCharicterizer(ftrace_input=input_file)
    # characterizer.parse_strace()
    # characterizer.parse_ftrace()
    # characterizer.graph_ftrace(10)
    # characterizer.graph_strace(10)

    # distb_plot("../tests/latency/same_location/client_timings.txt")
    bandwidth_plot("/Users/kdrahbar/workspaces/EECS_570/project/tests/max_bandwidth/same_location/bandwidth_timings.txt")
