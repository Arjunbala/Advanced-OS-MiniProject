# simple_pthread_timing.c
Simple workload that measures program time when 2 threads are spawned and compete for the same mutex.
Note that this program measures the CPU time.\
gcc -pthread simple_pthread_timing.c\
./a.out 0 10 [arg 1 - use spinlock or not, arg2 - time spent inside critical section]

# simple_pthread_timing_clocktime.c
Simple workload that measures program time when 2 threads are spawned and compete for the same mutex.
Note that this program measures the actual wall-clock time.\
gcc -pthread simple_pthread_timing_clocktime.c\
./a.out 0 10 [arg 1 - use spinlock or not, arg2 - time spent inside critical section]

# concurrency_workload.c
This is a composite workload that spawns threads based on the the value of NUM_THREADS specified during compilation. These threads are pinned onto cores as specified by the values of NUM_CORES during compilation. Threads are pinned onto cores - to be specific thread number "i" is pinned onto core i%NUM_CORES. Each thread does some work outside the critical section and some work inside the critical section. The timings for these works can be controlled through command line arguments.\
gcc -pthread concurrency_workload.c -DNUM_THREADS='16' -DNUM_CORES=8\
./a.out 0 10 10 [arg1 - use spinlock or not, arg2 - time outside critical section, arg3 - time inside critical section]
