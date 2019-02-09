clear_setup() {
    sync
    sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
}

for buffersize in 512 1024 2048 4096 8192 16384 32768 65536
    do
        gcc -std=c99 -o batch_task_random_writes batch_task_random_writes.c -DWRITE_SIZE=$buffersize
        gcc -std=c99 -o batch_task_random_writes_fsync batch_task_random_writes_fsync.c -DWRITE_SIZE=$buffersize
        clear_setup
        ./batch_task_random_writes > results/random_writes_benchmark/100gbfile/wofsync-bsize-$buffersize
        clear_setup
        ./batch_task_random_writes_fsync > results/random_writes_benchmark/100gbfile/wfsync-bsize-$buffersize
done
