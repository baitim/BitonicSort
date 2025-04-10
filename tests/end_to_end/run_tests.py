import os
import glob
import subprocess
import shutil
from pathlib import Path

tests_dir = Path(__file__).parent
build_dir = Path.cwd()

answer_dir = tests_dir / "answers"
tests_in_dir = tests_dir / "tests_in"

if answer_dir.exists():
    shutil.rmtree(answer_dir)
answer_dir.mkdir(parents=True, exist_ok=True)

exe_suffix = ".exe" if os.name == "nt" else ""
bitonic_sort_exe = build_dir / "../../src" / f"bitonic_sort{exe_suffix}"
bitonic_sort_exe = bitonic_sort_exe.resolve()

test_files = sorted(Path(tests_in_dir).glob("test_*.in"))

for test_num, file_path in enumerate(test_files, start=1):
    answer_path = answer_dir / f"answer_{test_num:03}.ans"

    with open(file_path, 'r') as input_file:
        input_data = input_file.read()

    result = subprocess.run(
        str(bitonic_sort_exe),
        input=input_data,
        capture_output=True,
        text=True,
        shell=True
    )

    with open(answer_path, 'w') as output_file:
        output_file.write(result.stdout.rstrip())

    print("test", test_num, "passed")
