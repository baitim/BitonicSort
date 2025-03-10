import os
import random
from pathlib import Path

tests_dir = str(Path(__file__).parent)

count_tests = 10
count_elems = 10000
max_value   = count_elems * 2

answer_dir = tests_dir + "/answers_core"
tests_dir  = tests_dir + "/tests_in"

os.system("rm -rf " + answer_dir)
os.system("mkdir "  + answer_dir)
os.system("rm -rf " + tests_dir)
os.system("mkdir "  + tests_dir)

for test_num in range(count_tests):
    test_file_name = tests_dir + "/test_" + f'{test_num+1:03}' + ".in"
    test_file = open(test_file_name, 'w')

    test_file.write(str(count_elems))
    test_file.write("\n")

    numbers = [random.randint(1, max_value) for _ in range(count_elems)]

    test_file.write(" ".join(map(str, numbers)))
    test_file.close()

    numbers.sort()

    answer_file_name = answer_dir + "/answer_" + f'{test_num+1:03}' + ".in"
    answer_file = open(answer_file_name, 'w')
    answer_file.write(" ".join(map(str, numbers)))

    answer_file.close()

    print("test ", test_num + 1, " generated")