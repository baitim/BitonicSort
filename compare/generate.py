import os
import random
import shutil
from pathlib import Path

tests_dir = str(Path(__file__).parent)

count_tests      = 5
count_elems_0    = 1024 * 102
count_elems_step = count_elems_0
max_value        = 10 * count_elems_0

tests_dir  = tests_dir + "/compare_in"

if os.path.exists(tests_dir):
    shutil.rmtree(tests_dir)
os.makedirs(tests_dir, exist_ok=True)

count_elems = count_elems_0
for test_num in range(count_tests):
    test_file_name = tests_dir + "/test_" + f'{test_num+1:03}' + ".in"
    Path(test_file_name).touch()
    test_file = open(test_file_name, 'w')

    count_elems *= 2

    test_file.write(str(count_elems))
    test_file.write("\n")

    numbers = [random.randint(1, max_value) for _ in range(count_elems)]

    test_file.write(" ".join(map(str, numbers)))
    test_file.close()

    print("test ", test_num + 1, " generated")
