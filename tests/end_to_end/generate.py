import random
from pathlib import Path

tests_dir = str(Path(__file__).parent)

count_tests = 10
count_elems = 10000
max_value   = count_elems * 2

for test_num in range(count_tests):
    file_name = tests_dir + "/tests_in/test_" + f'{test_num+1:03}' + ".in"
    file = open(file_name, 'w')

    file.write(str(count_elems) + "\n")
    for _ in range(count_elems):
        file.write(str(random.randint(1, max_value)) + " ")

    file.close()
    print("test ", test_num + 1, " generated")