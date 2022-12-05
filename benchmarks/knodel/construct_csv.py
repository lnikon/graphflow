import json
import glob
import csv
from datetime import datetime

def load_results_generator(my_path_regex):
    for filename in glob.glob(my_path_regex):
        for json_line in open(filename, 'r'):
            yield json.loads(json_line)

def write_csv(metrics):
    output_uniform_generation = f"uniform-generation.csv"
    output_uniform_pushrandomized = f"uniform-push-randomized.csv"
    output_uniform_broadcast = f"uniform-broadcast.csv"

    output_knodel_generation = f"knodel-generation.csv"
    output_knodel_pushrandomized = f"knodel-push-randomized.csv"
    output_knodel_broadcast = f"knodel-broadcast.csv"

    with open(output_uniform_generation, 'w') as f:
        wrtiter = csv.writer(f)
        for metric in metrics['uniform']:
            row = []
            row.append(metric['edges'])
            for data in metric['datasets']:
                row.append(data['mean'])
            
            wrtiter.writerow(row)

    with open(output_uniform_pushrandomized, 'w') as f:
        wrtiter = csv.writer(f)
        for metric in metrics['uniform']:
            row = []
            row.append(metric['edges'])
            for data in metric['pushrandomized']:
                row.append(data['mean'])
            
            wrtiter.writerow(row)

    with open(output_uniform_broadcast, 'w') as f:
        wrtiter = csv.writer(f)
        for metric in metrics['uniform']:
            row = []
            row.append(metric['edges'])
            for data in metric['broadcast']:
                row.append(data['mean'])
            
            wrtiter.writerow(row)

    with open(output_knodel_generation, 'w') as f:
        wrtiter = csv.writer(f)
        for metric in metrics['uniform']:
            row = []
            row.append(metric['edges'])
            for data in metric['datasets']:
                row.append(data['mean'])
            
            wrtiter.writerow(row)

    with open(output_knodel_pushrandomized, 'w') as f:
        wrtiter = csv.writer(f)
        for metric in metrics['uniform']:
            row = []
            row.append(metric['edges'])
            for data in metric['pushrandomized']:
                row.append(data['mean'])
            
            wrtiter.writerow(row)

    with open(output_knodel_broadcast, 'w') as f:
        wrtiter = csv.writer(f)
        for metric in metrics['uniform']:
            row = []
            row.append(metric['edges'])
            for data in metric['broadcast']:
                row.append(data['mean'])
            
            wrtiter.writerow(row)

def construct_metrics(path):
    results = [_json for _json in load_results_generator("./*.json")]
    metrics = {
        'uniform': [],
        'knodel': [],
    }

    for result in results: 
        model = result['model']
        edgeCount = result['counters']['edgeCount']['total']
        mean = result['counters']['generationTime']['mean']
        pushRandomizedMean = result['counters']['simulationTime']['randomizedPushGossip']['mean']
        broadcastMean = result['counters']['simulationTime']['broadcastGossip']['mean']
        nodes = result['nodes']
        totalVertexCount = result['totalVertexCount']
        percentage = result['percentage']

        found = False
        for idx, metric in enumerate(metrics[model]):
            if metric['totalVertexCount'] == totalVertexCount and metric['percentage'] == percentage:
                found = True
                metrics[model][idx]['datasets'].append({'nodes': nodes, 'mean': mean})
                metrics[model][idx]['pushrandomized'].append({'nodes': nodes, 'mean': pushRandomizedMean})
                metrics[model][idx]['broadcast'].append({'nodes': nodes, 'mean': broadcastMean})

                metrics[model][idx]['datasets'] = sorted(metrics[model][idx]['datasets'], key=lambda d: d['nodes'])
                metrics[model][idx]['pushrandomized'] = sorted(metrics[model][idx]['pushrandomized'], key=lambda d: d['nodes'])
                metrics[model][idx]['broadcast'] = sorted(metrics[model][idx]['broadcast'], key=lambda d: d['nodes'])
                break
        
        if not found:
            metrics[model].append({'edges': edgeCount, 'totalVertexCount': totalVertexCount, 'percentage': percentage,
                'datasets': [{'nodes': nodes, 'mean': mean}], 'pushrandomized': [{'nodes': nodes, 'mean': pushRandomizedMean}], 'broadcast': [{'nodes': nodes, 'mean': broadcastMean}]})
            print(f"{metrics[model]}\n")

        metrics[model] = sorted(metrics[model], key=lambda d: d['edges'])

    #print(f"{json.dumps(metrics)}")
    write_csv(metrics)

if __name__ == "__main__":
    construct_metrics("./*.json")
