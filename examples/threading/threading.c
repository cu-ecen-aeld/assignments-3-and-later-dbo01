#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>    

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)



/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    // do {
        res = nanosleep(&ts, &ts);
    // } while (res && errno == EINTR);

    return res;
}

void* threadfunc(void* thread_param)
{
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    int rc = 0;
    thread_func_args->thread_complete_success =  true;

    msleep(thread_func_args->wait_to_obtain_ms);

    rc = pthread_mutex_lock(thread_func_args->mutex);

    if (rc != 0 )
    {
        thread_func_args->thread_complete_success =  false;
        return thread_param;
    }

    msleep(thread_func_args->wait_to_release_ms);

    rc = pthread_mutex_unlock(thread_func_args->mutex);

    if (rc != 0 )
    {
        thread_func_args->thread_complete_success =  false;
        return thread_param;
    }

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{

    int rc = 0;
    void *p_thread_data = malloc(sizeof(struct thread_data));
    struct thread_data* p_shared_data = (struct thread_data *) p_thread_data;

    p_shared_data->mutex = mutex;
    p_shared_data->wait_to_obtain_ms = wait_to_obtain_ms;
    p_shared_data->wait_to_release_ms = wait_to_release_ms;


    if (rc != 0 ) return false;

    rc = pthread_create( thread, NULL,
        threadfunc, p_thread_data);

    if (rc != 0 ) return false;

    // free(p_thread_data);


    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    return true;
}
