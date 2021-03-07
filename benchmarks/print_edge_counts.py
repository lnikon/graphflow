#!/usr/bin/python3

vertexCount=[64,  256, 1024, 2048, 4096, 8192, 16384, 33668, 67336, 134672,  269344,   538688, 1077376]
percentages=[100, 100, 100,  50,   15,   5,    3,     1,     0.5,   0.2,     0.08,     0.03,   0.009]

for idx, v in enumerate(vertexCount):
  print ('{0} {1:15}'.format(((v * v) / 2) * (percentages[idx] / 100), v))
  
