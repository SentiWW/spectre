import subprocess
import time
from concurrent.futures import ThreadPoolExecutor

def run_perf_command(event):
    cmd = f"perf stat -e {event} --timeout 100 -x,"
    output = subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT)
    return int(output.decode().split(',')[0])

def write_results_to_file(file_path, cache_misses, cache_references, branch_instructions, branch_misses, cache_miss_rate, branch_miss_rate):
    with open(file_path, "a") as file:
        file.write(f"{cache_misses};{cache_references};{branch_instructions};{branch_misses};{cache_miss_rate};{branch_miss_rate}\n")

def main():
    events = ["cache-misses", "cache-references", "branch-instructions", "branch-misses"]
    file_path = "output-measure.csv"

    with ThreadPoolExecutor() as executor:
        # Execute the perf commands in parallel
        results = executor.map(run_perf_command, events)

    # Extract the results
    cache_misses, cache_references, branch_instructions, branch_misses = results
    cache_miss_rate = cache_misses / cache_references
    branch_miss_rate = branch_misses / branch_instructions

    # Write results to a file
    write_results_to_file(file_path, cache_misses, cache_references, branch_instructions, branch_misses, cache_miss_rate, branch_miss_rate)

if __name__ == "__main__":
    with open("output-measure.csv", "a") as file:
        file.write("start time;end time;cache misses;cache references;branch instructions;branch misses;cache miss rate;branch miss rate\n")

    for i in range(1200):
        main()
