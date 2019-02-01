#include <pthread.h>
#include <stdio.h>

pthread_mutex_t lock;
pthread_spinlock_t spinlock;

struct thread_args {
    int func_number;
    int sleep_time_usec;
    int useSpinlock;
};

void *sleep_routine(void *args) {
    struct thread_args *thread_args = (struct thread_args *) args;
    if(thread_args->useSpinlock) {
	int ret;
	do {
            ret = pthread_spin_trylock(&spinlock);
	} while(ret != 0);
    } else {
	pthread_mutex_lock(&lock);
    }
    printf("Starting sleep thread %d\n", thread_args->func_number);
    usleep(thread_args->sleep_time_usec);
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
    clock_t start,end;
    start = clock();
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

    end = clock();
    double cpu_time_used = ((double) (end - start));
    printf("CPU time used = %lf\n", cpu_time_used);
    return 0;
}
