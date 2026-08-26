# Electric Vehicle Routing Problem #

### Source code
* evrp.cpp
Implementation of the EVRP Problem

...


/** Execution Instructions **/ 

Step 1: Access to the path which containing the source code files with command line(CMD). 
Here are the commands necessary to run our c++ code in CMD.


Step 2: Run this command in CMD (To create an executable file named "file_name"): 
```
$ cmake -S . -B build
$ cmake --build build
```

Step 3: Run this command in CMD (To run the executable file):
```
$ ./build/evrp GS data/E-n22-k4.evrp
```

The solution is written to `output_files/1/solution_GS_E-n22-k4.evrp.txt`, using
the same format as `evrp_neih4207`.

/** Test Execution Instructions **/ 

```
$ cmake -S . -B build
$ cmake --build build
$ ctest --test-dir build --output-on-failure
```

/** Plot Solution Instructions **/ 

```
$ source .venv/bin/activate
$ python3 evrpgraph.py -i output_files/1/solution_GS_E-n22-k4.evrp.txt
```
