<h1 align="center">BitonicSort</h1>

## Description

 Implementation of the bitonic sort with OpenCL.

## How to integrate
 
 use [storage](https://github.com/baitim/ConanPackages), project = "bitonic_sort", version = "1.0", user = "baitim"

## How to run

1. Clone <br>
    <code>git clone https://github.com/baitim/BitonicSort.git</code>

2. Go to folder <br>
    <code>cd BitonicSort</code>

3. Prepare conan <br>
    <code>uv sync --group dev; source .venv/bin/activate</code><br>
    <code>conan profile detect --force</code>

4. Init dependencies <br>
    <code>conan install . --build=missing -s compiler.cppstd=gnu20</code><br>
    maybe you will need these flags for the conan <code>-s build_type=Debug</code>

5. Build <br>
    <code>cmake --preset release; cmake --build build/Release</code>

6. Run <br>
    <code>./build/Release/src/bitonic_sort</code>

## How to test

* Testing
    - End to end & Unit<br>
        <code>ctest --test-dir build/Release --output-on-failure</code>

## How to compare

* Preparing
    write <code>cd compare</code> in terminal <br>
    write <code>python3 generate.py</code> in terminal <br>

* Comparing
    write <code>python3 compare.py</code> in terminal <br>
    The result of the comparison will be in the "compare.out" and "times.png" file

<figure>
    <p align="center"><img src="https://github.com/baitim/AVLTree/blob/main/compare/times.png" width="80%"></p>
    <figcaption><p align="center">i5-1135G7 & Intel® Xe Graphics</p></figcaption>
</figure>

<p align="center"><img src="https://github.com/baitim/BitonicSort/blob/main/images/cat.gif" width="40%"></p>

## Support
**This project is created by [baitim](https://t.me/bai_tim)**