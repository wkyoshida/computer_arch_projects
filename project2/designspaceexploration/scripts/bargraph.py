#!/usr/bin/env python

# William Yoshida
# CMPEN 431
# Design Space Exploration Project
## Script for extracting and plotting data from .best files

# libraries
import matplotlib.pyplot as plt; plt.rcdefaults
import numpy as np
import sys
import matplotlib.pyplot as plt

# initialization
value = []
objects = []

# specify file and data type
fileIndex = int(sys.argv[1]) # EnergyEfficiency (1) or ExecutionTime (0)
if (fileIndex == 0):
    typePlot = 'ExecutionTime'
    dataIndex = [2, 6, 8, 10, 12, 14];
else:
    typePlot = 'EnergyEfficiency'
    dataIndex = [1, 6, 8, 10, 12, 14];

fileName = typePlot + '.best'

# get data from file
with open(fileName, "r") as filestream:
    for line in filestream:

        # go to next line for ExecutionTime data
        if (fileIndex == 0):
            continue
        currentline = line.split(",")
        for j in dataIndex:
            value.append( float(currentline[j]) );
        break

y_pos = np.arange(len(value))

# name labels
for i in y_pos:
    if (i == 0):
        objects.append('Geo');
    else:
        objects.append('BM ' + str(i));

# plot data
plt.bar(y_pos, value, align='center', alpha=0.5)
plt.xticks(y_pos, objects)
plt.ylabel('Normalized ' + typePlot)
plt.title('Normalized ' + typePlot + 's')
plt.show()
