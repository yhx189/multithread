#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include "thread_pool.h"
#include "semaphore.h"
/**
 *  @struct threadpool_task
 *  @brief the work struct
 *
 *  Feel free to make any modifications you want to the function prototypes and structs
 *
 *  @var function Pointer to the function that will perform the task.
 *  @var argument Argument to be passed to the function.
 */

#define MAX_THREADS 20
#define STANDBY_SIZE 8
typedef struct __arg_t{
char *buf;
int bufsize;
int customer_id;
int seat_id;
int customer_priority;
}arg_t;

typedef struct {
    void (*function)(void *);
    void *argument;
} pool_task_t;


struct pool_t {
  m_sem_t  sem;
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_t *threads;
  pool_task_t *queue;
  int thread_count;
  int task_queue_size_limit;
};

static void *thread_do_work(void *pool);
pthread_attr_t attr;
pool_task_t *queue_head;
int count = 0;
/*
 * Create a threadpool, initialize variables, etc
 *
 */
pool_t *pool_create(int queue_size, int num_threads)
{
    struct pool_t *ret = (pool_t*)malloc(sizeof(struct pool_t));
    pthread_mutex_init(&ret->lock, NULL);
    sem_init(&ret->sem, 0);
    pthread_cond_init(&ret->notify, NULL);
    ret->threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ret->queue = (pool_task_t*)malloc(queue_size * sizeof(pool_task_t));
    int i;
   /* for(i = 0; i < queue_size; i++)
    {
	    ret->queue[i].function = NULL;
	    ret->queue[i].argument = NULL;
    }
*/
    queue_head = ret->queue;
    if (num_threads > MAX_THREADS){
    	fprintf(stderr, "exceed max threads\n");
    
    }
    ret->thread_count = num_threads;
    ret->task_queue_size_limit = queue_size;
    for (i=0; i< num_threads;i++){
    	pthread_create( & (ret->threads[i]), NULL, thread_do_work, (void*)ret);
    }
   return ret;

}


/*
 * Add a task to the threadpool
 *
 */
int pool_add_task(pool_t *pool, void (*function)(void *), void *argument)
{
    int err = 0;
    pthread_mutex_lock(&pool->lock);
   // printf("add task, count = %d\n", count);
    if (count > pool->task_queue_size_limit){
    	err = 1;
	fprintf(stderr, "exceed task queue size limit\n");
	return err;
    }
    pool_task_t *ret = (pool_task_t*)malloc(sizeof(pool_task_t));
    ret->function = function;
    ret->argument = argument;
   // printf("before: func = %p\n", function);
   // printf("befpre: argument = %p\n", argument);
    pool->queue [count] = *ret;//sizeof(ret);
    count++;
   // if ( count > 0) {
	//    pthread_cond_signal(&pool->notify);
    //}
     sem_post(&pool->sem);
     pthread_mutex_unlock(&pool->lock);
     printf("sem => %d\n", pool->sem.value);

    return err;
}



/*
 * Destroy the threadpool, free all memory, destroy treads, etc
 *
 */
int pool_destroy(pool_t *pool)
{
    int err = 0;
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
    int i;
    for(i = 0; i < pool->thread_count; i++)
    	pthread_cancel(pool->threads[i]);
    return err;
}



/*
 * Work loop for threads. Should be passed into the pthread_create() method.
 *
 */
static void *thread_do_work(void *pool)
{ 

    // if there is no task, wait
    // else pop the queue and allocate a thread
    pthread_attr_init(&attr);
    int i;
    pool_t * _pool = (pool_t* )pool;
    while(1){
     // pthread_mutex_lock(&(_pool->lock));

     // while ( count <= 0 ){
	//  pthread_cond_wait(&(_pool->notify), &(_pool->lock));
     // }

      sem_wait(&_pool->sem);
      printf("sem => %d\n", _pool->sem.value);
      //printf("after wait\n");
   //   if (count > 0){
		void (*func)(void *) = NULL;
		func = (_pool->queue[0]).function;
    		void * argument = (_pool->queue[0]).argument;
		func(argument);
		count--;

		for(i = 0; i < count; i++)
	     		_pool->queue[i] = _pool->queue[i+1];
                for (i = count ; i < _pool->task_queue_size_limit; i++){
			_pool->queue[i]. function = NULL; 
			_pool->queue[i]. argument = NULL;
	        //       pool_task_t* temp = (pool_task_t*)malloc(sizeof(pool_task_t));
		       
		  //     _pool->queue[i] = *temp;             
		}
     // }
   
    //  pthread_mutex_unlock(&_pool->lock);

   } 
    pthread_exit(NULL);
    return NULL;
}   
