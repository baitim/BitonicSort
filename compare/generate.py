import random
from pathlib import Path

tests_dir = str(Path(__file__).parent)

count_tests = 10
count_elems = 50000
max_value   = count_elems * 2

tests_dir  = tests_dir + "/compare_in"

for test_num in range(count_tests):
    test_file_name = tests_dir + "/test_" + f'{test_num+1:03}' + ".in"
    test_file = open(test_file_name, 'w')

    count_elems *= 2

    test_file.write(str(count_elems))
    test_file.write("\n")

    numbers = [random.randint(1, max_value) for _ in range(count_elems)]

    test_file.write(" ".join(map(str, numbers)))
    test_file.close()

    print("test ", test_num + 1, " generated")