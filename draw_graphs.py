#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt


def draw_times(giant_increase, filename):
    fig = plt.figure()
    ax = fig.add_axes([0, 0, 1, 1])
    positions = [i for i in range(8)]
    cores = ["{} cores".format(i + 1) for i in range(8)]

    plt.bar(positions, giant_increase, align='center', alpha=0.5)
    plt.xticks(positions, cores)
    plt.ylabel("Increase")
    plt.title("Big tests threading increase")
    plt.savefig(filename, bbox_inches='tight')

small_single = 1.65
small = [
    2.71,
    42.76,
    65.68,
    40.69,
    61.47,
    68.95,
    38.01,
    50.12,
]

large_single = 32925.73333333333
large = [
    164992.8,
    134506.9,
    112251.13333333333,
    101215.63333333333,
    98583.13333333333,
    91325.4,
    91455.93333333333,
    94972.03333333334,
]

arr_small = [small_single / el for el in small]
draw_times(arr_small, 'Pictures/small_increase.png')
arr_large = [large_single / el for el in large]
draw_times(arr_large, 'Pictures/large_increase.png')
