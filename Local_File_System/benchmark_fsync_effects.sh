clear_setup() {
    sync
    sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
    rm -rf testfile.txt
}

for filesize in 1024 10240 102400
do
    for buffersize in 1024 2048 4096 8192 16384 32768 65536
    do
        gcc -std=c99 -o batch_task_fsync batch_task_fsync.c -DRANDOM_STRING_SIZE=$buffersize -DINPUT_FILE_SIZE_KB=$filesize
        clear_setup
        ./batch_task_fsync 0 > results/fsync_effects/buffered-write-fsize$filesize-bsize$buffersize
        clear_setup
        ./batch_task_fsync 1 > results/fsync_effects/direct-write-fsize$filesize-bsize$buffersize
    done
done
