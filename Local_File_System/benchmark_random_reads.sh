clear_setup() {
    sync
    sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
}

for buffersize in 512 1024 2048 4096 8192 16384 32768 65536
    do
        gcc -std=c99 -o batch_task_random_reads batch_task_random_reads.c -DREAD_SIZE=$buffersize
        clear_setup
        ./batch_task_random_reads > results/random_reads_benchmark/direct_alone/randomread-bsize-$buffersize
done
