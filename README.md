# perflib

perflib provides fast instrumentation and tracing API to generate trace data during execution.

Travis CI: [![Build Status](https://travis-ci.org/xinhuang/perflib.svg)](https://travis-ci.org/xinhuang/perflib)
AppVeyor CI: [![Build Status](https://ci.appveyor.com/api/projects/status/qa62sqkufd5fey0y?svg=true)](https://ci.appveyor.com/project/xinhuang/perflib)

## Example

```
PerfModule *M = perf_module("Module A");
PerfTask *A = perf_task("Task A");
PerfTask *B = perf_task("Task B");

using namespace tbb;
task_scheduler_init TSI(1);

task_group Group;

perf::start(M, A);
Group.run_and_wait([]() {
  task_group Group;
  Group.run_and_wait([]() {
    perf::start(M, B);
    // Busy processing data
    perf::stop(M);
  });
});
perf::stop(M);

std::ofstream PerfReport("./PerfReport.txt");
perf::report(PerfReport);
```

*Output*
```
Total Time: 1.00055 seconds.
	Module Module A: 1.00054 seconds.
		Task Task A : 1.00054 seconds.
		Task Task B : 1.00019 seconds.
```

## How to Build

### Set Up Dependencies

* TBB  

1. Download TBB distribution from https://www.threadingbuildingblocks.org/  
2. Build TBB from source the distribution doesn't target to your compiler  

### Build

* Linux

```
  git clone git@github.com/xinhuang/perflib.git
  cd perflib
  mkdir build
  cd build
  cmake ..
  make
  make test                             # Run tests
```

* Windows

```
  git clone git@github.com/xinhuang/perflib.git
  cd perflib
  mkdir build
  cd build
  cmake .. -G "Visual Studio 14 2015 Win64"   # VS 2015
  msbuild perflib.sln /m
  msbuild RUN_TESTS.vcxproj             # Run tests
```

* x86-64

If you prefer x86-64,
*For Windows*, use -G "Visual Studio 14 2015 Win64".


### Toubleshooting

*If Boost is not found*,  
Add `-DBOOSTROOT=<your boost include path>`. On Windows, by default it's `C:\Boost\include\boost-1_59`.  

*If TBB is not found*,  
Add `-DTBBROOT=<your TBB root path` and set environment variable `TBB_ARCH_PLATFORM`
match your platform and compiler. For example, `ia32/vc14` is for VC14 on x86, `intel64/gcc4.8` for gcc 4.8 on x86-64.
You can also get these variables set by running `<TBBROOT>/tbbvars.bat <platform> <compiler>`.

## TODO

* Collect running time for each module  
* Use pipeline to parallel read/comparison  
* Report differences  
* Blob file support  
* String type support
* Out-of-order column support
* Data range shifts detection and comparison support
* Missing data file detection and comparison support

## Coding Style

This project follows the [LLVM Coding Standards](http://llvm.org/docs/CodingStandards.html).
