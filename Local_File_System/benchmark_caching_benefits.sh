clear_setup() {
    sync
    sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
    rm -rf testfile.txt
}

for filesize in 10240 102400 1048576
do
    for buffersize in 512 1024 2048 4096 8192 16384 32768 65536
    do
        gcc -std=c99 -o batch_task_caching_benefits batch_task_caching_benefits.c -DRANDOM_STRING_SIZE=$buffersize -DINPUT_FILE_SIZE_KB=$filesize
        clear_setup
        ./batch_task_caching_benefits 0 > results/caching_benefits/buffered-write-fsize$filesize-bsize$buffersize
        clear_setup
        ./batch_task_caching_benefits 1 > results/caching_benefits/direct-write-fsize$filesize-bsize$buffersize
    done
done
