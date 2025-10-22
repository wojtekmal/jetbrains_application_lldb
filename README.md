# gwatch
A command line tool to watch system variables accessed by processes.  
Synopsis:  
```gwatch --var <symbol> --exec <path> [-- arg1 ... argN]```  
## Building
The ```gwatch``` executable can be found in ```build/bin```. It may be placed in a $PATH directory or just executed with ```./build/bin/gwatch```. The project may be built with
```
cmake -B build
cmake --build build
```
Requirements: Cmake, G++, Linux.  
## Tests
Tests may be run with ```cd build && ctest```. A performance test may be run by running the ```measure_perf.sh``` script.  
  
## Example usage
  
Using gwatch to check a program that increments an integer:

```
build/bin/gwatch --var global_int_to_watch --exec test_programs/test_int_incrementation
```

The above example is included in the tests and may also be run with ```ctest```.