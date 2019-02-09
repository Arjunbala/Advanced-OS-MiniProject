import numpy as np
import matplotlib.pyplot as plt
import json
import sys
import os

def get_read_times(filename):
    data = [int(line.strip()) for line in open(filename, 'r')]
    return data

def plot_e2e_times(list_of_points, color, label):
    sorted_vals = np.sort(list_of_points)
    p = 1. * np.arange(len(sorted_vals))/(len(sorted_vals) - 1)
    plt.plot(sorted_vals, p, color, label=label)

points_read_buffered_512 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/512_buffered")
points_read_direct_512 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/512_direct")
points_read_buffered_1024 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/1024_buffered")
points_read_direct_1024 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/1024_direct")
points_read_buffered_2048 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/2048_buffered")
points_read_direct_2048 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/2048_direct")
points_read_buffered_4096 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/4096_buffered")
points_read_direct_4096 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/4096_direct")
points_read_buffered_8192 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/8192_buffered")
points_read_direct_8192 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/8192_direct")
points_read_buffered_16384 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/16384_buffered")
points_read_direct_16384 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/16384_direct")
points_read_buffered_32768 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/32768_buffered")
points_read_direct_32768 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/32768_direct")
points_read_buffered_65536 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/65536_buffered")
points_read_direct_65536 = get_read_times("/users/balarjun/Advanced_OS/Advanced-OS-MiniProject/Local_File_System/results/random_reads_benchmark/65536_direct")

plot_e2e_times(points_read_buffered_512, 'r', 'Buffered 512')
plot_e2e_times(points_read_direct_512, 'r--', 'Direct 512')
plot_e2e_times(points_read_buffered_1024, 'b', 'Buffered 1024')
plot_e2e_times(points_read_direct_1024, 'b--', 'Direct 1024')

plt.legend()
plt.xlabel('End to End Latencies in ms', fontsize=18)
plt.savefig("e2e_cdf.png")
