#!/usr/bin/env python

# William Yoshida
# CMPEN 431
# Design Space Exploration Project
## Script for extracting and plotting data from .log files

# libraries
import matplotlib.pyplot as plt
import sys

# initialization
i = 0
x = []
y = []

# specify file and data type
typePlot = str(sys.argv[1]) # EnergyEfficiency or ExecutionTime
fileName = typePlot + '.log'
fileIndex = int(sys.argv[2]) # index for data within file

# get data from file
with open(fileName, "r") as filestream:
    for line in filestream:
        currentline = line.split(",")
        i += 1
        y.append( float(currentline[fileIndex]) );
        x.append(i);

# plot data
plt.plot(x, y)
plt.xlabel('Number of designs')
plt.ylabel('Normalized' + typePlot)
plt.title('Normalized Geomean ' + typePlot + ' Progression')
plt.show()
