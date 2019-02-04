#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define SEC_TO_NS 1000000000

pthread_mutex_t lock;
pthread_spinlock_t spinlock;

struct thread_args {
    int func_number;
    int sleep_time_usec;
    int useSpinlock;
};

void keep_cpu_busy(int time_usec) {
    int count = 0;
    while(count < time_usec) {
        usleep(1);
        count++;
    }
}

void *sleep_routine(void *args) {
    struct thread_args *thread_args = (struct thread_args *) args;
    if(thread_args->useSpinlock) {
	int ret;
	pthread_spin_lock(&spinlock);
    } else {
	pthread_mutex_lock(&lock);
    }
    printf("Starting sleep thread %d\n", thread_args->func_number);
    keep_cpu_busy(thread_args->sleep_time_usec);
    printf("Finished sleep thread %d\n", thread_args->func_number);
    if(thread_args->useSpinlock) {
        pthread_spin_unlock(&spinlock);
    } else {
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int useSpinlock = atoi(argv[1]);
    int sleep_time_usec = atoi(argv[2]);
    if(useSpinlock) {
        pthread_spin_init(&spinlock, 0);
    }
    pthread_t sleep_thread_1, sleep_thread_2;

    struct thread_args t1_args = {1, sleep_time_usec, useSpinlock};
    struct thread_args t2_args = {2, sleep_time_usec, useSpinlock};
    struct timespec start,end;
    
    clock_gettime(CLOCK_REALTIME, &start);
    if (pthread_create(&sleep_thread_1, NULL, sleep_routine, &t1_args)) {
        printf("Error in creating sleep thread\n");
	return 1;
    }
    if (pthread_create(&sleep_thread_2, NULL, sleep_routine, &t2_args)) {
        printf("Error in creating sleep thread\n");
        return 1;
    }
    
    if(pthread_join(sleep_thread_1, NULL)) {
        printf("Error joining thread\n");
        return 1;
    }
    if(pthread_join(sleep_thread_2, NULL)) {
        printf("Error joining thread\n");
        return 1;
    }
    clock_gettime(CLOCK_REALTIME, &end);

    unsigned long long wall_time_ns = (unsigned long long) ((end.tv_sec - start.tv_sec)*SEC_TO_NS) + (end.tv_nsec - start.tv_nsec);
    printf("Wall time used = %lld\n", wall_time_ns);
    return 0;
}
