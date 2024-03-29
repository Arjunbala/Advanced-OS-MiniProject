# batch_task.c
Depending on argument passed in (0 for buffered I/O and 1 for direct I/O), the program writes a 10MB file sequentially in a child process. The parent process then reads it.\
gcc -o batch_task batch_task.c -DRANDOM_STRING_SIZE='4096'\
./batch_task 0

# batch_task_caching_benefits.c
The program writes a 10MB file sequentially using buffered I/O or direct I/O depending on the argument. The parent process reads the file using buffered I/O.\
gcc -o batch_task_caching_benefits batch_task_caching_benefits.c -DRANDOM_STRING_SIZE='4096'\
./batch_task_caching_benefits 0

# batch_task_fsync.c
The program writes a 10MB file sequentially using Direct I/O or Buffered I/O. However, in Buffered I/O, the program uses fsync() to flush the contents to disk after each individual write. Then the parent process reads the file using buffered I/O.\
gcc -o batch_task_fsync batch_fsync.c -DRANDOM_STRING_SIZE='4096'\
./batch_task_fsync 0

# batch_task_random_writes.c
This program writes to random offsets in a file.\
gcc -o batch_task_random_writes batch_task_random_writes.c -DWRITE_SIZE='512'
./batch_task_random_writes

# batch_task_random_writes_fsync.c
This program writes to random offsets in a file while doing an fsync at the end of each write.\
gcc -o batch_task_random_writes_fsync batch_task_random_writes_fsync.c -DWRITE_SIZE='512'
./batch_task_random_writes_fsync

# batch_task_random_reads.c
This program reads a file using buffered and direct I/O and compares the read bandwidths.\
gcc -o batch_task_random_reads batch_task_random_reads.c -DREAD_SIZE='512'
./batch_task_random_reads\
Ensure that you run the below command below running the test to clear any buffer caches -\
sudo sh -c "echo  3 > /proc/sys/vm/drop_caches"
