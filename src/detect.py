import subprocess
from concurrent.futures import ThreadPoolExecutor
import joblib
import pandas as pd

def run_perf_command(event):
    cmd = f"perf stat -e {event} --timeout 100 -x,"
    output = subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT)
    return int(output.decode().split(',')[0])

def main():
    events = ["cache-misses", "cache-references", "branch-instructions", "branch-misses"]

    total_cache_misses, total_cache_references, total_branch_instructions, total_branch_misses = 0, 0, 0, 0

    for i in range(10):
        with ThreadPoolExecutor() as executor:
            # Execute the perf commands in parallel
            results = executor.map(run_perf_command, events)

        # Extract the results
        cache_misses, cache_references, branch_instructions, branch_misses = results
        total_cache_misses += cache_misses
        total_cache_references += cache_references
        total_branch_instructions += branch_instructions
        total_branch_misses += branch_misses

    cache_misses = total_cache_misses / 10
    cache_references = total_cache_references / 10
    branch_instructions = total_branch_instructions / 10
    branch_misses = total_branch_misses / 10

    cache_miss_rate = total_cache_misses / total_cache_references
    branch_miss_rate = total_branch_misses / total_branch_instructions

    print(f"Cache miss rate: {cache_miss_rate}")
    print(f"Branch miss rate: {branch_miss_rate}")

    model = joblib.load("logistic_regression_model.joblib")

    # Define the new data point
    new_data = {
        'cache miss rate': cache_miss_rate,
        'branch miss rate': branch_miss_rate
    }

    # Create a dataframe from the new data
    df_new_data = pd.DataFrame([new_data])

    # Prepare the new data for prediction
    X_new_data = df_new_data[['cache miss rate', 'branch miss rate']]

    # Make predictions on the new data
    prediction = model.predict(X_new_data)

    # Print the prediction
    if prediction[0] == 1:
        print("Attack!")

if __name__ == "__main__":
    while True:
        main()
