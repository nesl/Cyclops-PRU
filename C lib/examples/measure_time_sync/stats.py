import sys
import numpy

filename = sys.argv[1]
f = open(filename, "r")

diff = []

for line in f:
    t = line.split(",")
    qot_ts = int(t[0])
    pru_ts = int(t[1])
    diff.append(pru_ts - qot_ts)

print("mean: %s" % numpy.mean(diff))
print("stdev: %s" % numpy.std(diff))
print("median: %s" % numpy.median(diff))
