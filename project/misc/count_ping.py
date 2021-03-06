import pylab as plt
import operator
import math

input_file = "CE_strace_server.txt"

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
        LABELS = [x[0] for x in sorted_counts]

        fig = plt.figure()
        plt.bar(position, total_times, align='center')
        plt.xticks(position, LABELS, fontsize=5)
        fig.suptitle('GCE Server Time Spent in Kernel Calls', fontsize=20)
        plt.xlabel('Funciton Name', fontsize=12)
        plt.ylabel('Time (us)', fontsize=12)
        plt.show()


if __name__ == "__main__":
    characterizer = NetworkCharicterizer(strace_input=input_file)
    characterizer.parse_strace()
    # characterizer.parse_ftrace()
    
    characterizer.graph_strace(10)
