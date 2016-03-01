input_file = "ping_output.txt"

class NetworkCharicterizer():

    def __init__(self, s_input_file):
        self.strace_input_file = input_file

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


if __name__ == "__main__":
    ping_stats = NetworkCharicterizer(input_file)
    ping_stats.parse_strace()
    print ping_stats.strace_counts