#import argparse
import subprocess
import time
import os
import tempfile as tf

SMP_EXECUTABLE_PATH="../../build/src/PGASGraphCLI/pgas-graph-cli-smp"
#NUMBER_OF_CORES_TO_TEST = [2**x for x in range(0, 5)]
NUMBER_OF_CORES_TO_TEST = [16]

def construct_commandline_arguments(vertexNumber=128, percentage=5, model="uniform", algorithm="randomizedpushgossip", doExportGraph=False, doExportMetrics=True, metricsExportPath=""):
    result = []
    result.append(f"--vertex-count={vertexNumber}")
    result.append(f"--percentage={percentage}")
    result.append(f"--model={model}")
    result.append(f"--algorithm={algorithm}")
    result.append(f"--metrics-json-path={metricsExportPath}")
    return result

def construct_env(nodes=1):
    env = {**os.environ, 'GASNET_PSHM_NODES': str(nodes)}
    return env

def run_smp_executable(exec_path, env, arguments):
    cmd_line = [exec_path] + arguments
    print(f"cmd_line={' '.join(cmd_line)}")

    p = subprocess.Popen(cmd_line, env=env, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    while p.poll() is None:
        continue
    ret = p.poll()
    print(p.stdout.read())
    print(p.stderr.read()) # Catch remaining output
    p.stdout.close() # Always close your file handles, or your OS might be pissed
    p.stderr.close()
    p.kill()

    time.sleep(1)
    
    pass

def run_benchmark(nodes=1, vertexNumber=128, percentage=5, model="uniform", algorithm="randomizedpushgossip", doExportGraph=False, doExportMetrics=True):
    metrics_export_path=f"nodes-{nodes}-model-{model}-vertexNumber-{vertexNumber}-percentage-{percentage}-algorithm-{algorithm}.json"
    print(f"Running benchmarks with {vertexNumber} verticces, {percentage} percentage, {algorithm} algorithm, and {model} model\n")
    print(f"Metics will be exported into the following path: {metrics_export_path}")

    arguments = construct_commandline_arguments(vertexNumber=vertexNumber, percentage=percentage, model=model, algorithm=algorithm, metricsExportPath=metrics_export_path)
    env = construct_env(nodes)
    run_smp_executable(SMP_EXECUTABLE_PATH, env=env, arguments=arguments)
    pass

if __name__ == '__main__':
    print(f"Running benchmarks with executable \"{SMP_EXECUTABLE_PATH}\".\nFollowing core numbers will be used {NUMBER_OF_CORES_TO_TEST}")

    #number_of_vertices = [64**x for x in range(1, 11)]
    #percentages = list(range(1, 50, 4))
    number_of_vertices = [128]
    percentages = [5, 10] 
    models = ["uniform", "knodel"]
    algorithms = ["randomizedpushgossip", "broadcastgossip"]

    for nodes in NUMBER_OF_CORES_TO_TEST:
        for algorithm in algorithms:
            for model in models:
                for vertices in number_of_vertices:
                    if model == "uniform":
                        for percentage in percentages:
                            run_benchmark(nodes=nodes, algorithm=algorithm, percentage=percentage, model=model, vertexNumber=vertices)
                    elif model == "knodel":
                        run_benchmark(nodes=nodes, algorithm=algorithm, model=model, vertexNumber=vertices)
    pass
