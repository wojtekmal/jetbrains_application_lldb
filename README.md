# gwatch
A command line tool to watch system variables accessed by processes.  
Synopsis:  
```gwatch --var \<symbol> --exec \<path>```  
  
The ```gwatch``` executable can be found in ```build/bin```. It may be placed in a $PATH directory or just executed with ```./build/bin/gwatch```. The project may be built with ```cmake --build build```. Requirements: Cmake, G++, Linux.  
  
Tests may be run with ```cd build && ctest```.  
  
For now, only unsigned 4-8 byte integer global variables are supported.  
  
Example usage may be viewed in the invocation test script (```invocation_test.sh```).