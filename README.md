# CS241-Advanced-Compiler-Construction-Project
This is the class project for Michael Franz's CS 241 Advanced Compiler Construction course at UC Irvine. The goal of the project is to build an optimizing compiler for a simple programming language.

## Build Environment
* Ubuntu 19.10  
* gcc 9.2.1
* Make 4.2.1

## Usage
* compile
```
make
```
* run on either public or custom testcases with
```
make run_separate_public
```
or
```
make run_separate_custom
```
* can also run on any combination of files
```
./compiler <file1> <file2> ... <fileN>
```

## Output Visualization
The output is saved in `graphml/` after running the program. The results are in graphml format and are guarenteed compatible with [yEd 3.19.1.1](https://www.yworks.com/products/yed) on ubuntu. It should be compatible with other versions of yEd or [yEd live](https://www.yworks.com/yed-live/).  

It is recommended that when viewing output, the user should first do `tools -> fit node to label` and then `alt + shit + h` or `layout -> hiercharal` 
  
All output is in SSA format, where nodes are basic blocks and a directed edge from A to B means B is a successor to A. If a line is in the format `R0 = {instruction}`, that means the output of the instruction has been assigned to register 0. 
  
The output is saved after the first pass of SSA generation, which includes CSE, copy propagation, and constant folding. It is also saved after register allocation.  
  
Additionally, the interference graph is saved after the last iteration of its construction, although it is only readable on smaller programs.
