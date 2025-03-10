import os
import glob
import subprocess
import matplotlib.pyplot as plt
from pathlib import Path

cmp_dir = str(Path(__file__).parent)

cmp_bitonic_sort_exe = cmp_dir + "/../build/Release/compare/cmp_bitonic_sort"
files = list(map(str, glob.glob(cmp_dir + "/compare_in/test_*.in")))
files.sort()

name_out_file = cmp_dir + "/compare.out"
os.system("touch " + name_out_file)
os.system("echo -n > " + name_out_file)
out_file = open(name_out_file, 'w')
out_file.write(f"{'file':<20}{'count':<10}{'gpu_time':<12}{'cpu_time':<12}{'sort_time':<12}\n")

test_num = 0
counts = []
gpu_times  = []
cpu_times  = []
sort_times = []
for file in files :

    command = cmp_bitonic_sort_exe + " < " + file
    output = subprocess.check_output(command, shell=True).decode("utf-8")
    count, gpu_time, cpu_time, sort_time = map(float, output.splitlines())

    time_str = f"{Path(file).name:<20}{int(count):<10}{gpu_time:<12.3f}{cpu_time:<12.3f}{sort_time:<12.3f}\n"
    out_file.write(time_str)

    counts.append(count)
    gpu_times.append(gpu_time)
    cpu_times.append(cpu_time)
    sort_times.append(sort_time)
    test_num += 1
    print("test",  test_num, "passed")

out_file.close()

plt.plot(counts, gpu_times,  marker='o', label='bitonic_sort_gpu')
plt.plot(counts, cpu_times,  marker='o', label='bitonic_sort_cpu')
plt.plot(counts, sort_times, marker='o', label='sort')
plt.title("Times")

plt.xlabel('count')
plt.ylabel('time, ms')

plt.legend()
plt.savefig(cmp_dir + "/times.png")