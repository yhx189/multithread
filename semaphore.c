#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


typedef struct m_sem_t {
    int value;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} m_sem_t;

int sem_wait(m_sem_t *s);
int sem_post(m_sem_t *s);
void sem_init(m_sem_t *s, int value){
     
     pthread_mutex_init(&(s->lock), NULL);
     pthread_cond_init(&(s->cond), NULL);
     s->value = value;
}
void* sem_clean(void* arg)
{
   pthread_mutex_t *mutex = (pthread_mutex_t*)arg;
   pthread_mutex_unlock(mutex);
   return NULL;

}
int sem_wait(m_sem_t *s)
{
    pthread_mutex_lock(&(s->lock));
    pthread_cleanup_push(sem_clean, (void*)&(s->lock));
    while(s->value <= 0){
	    pthread_cond_wait(&(s->cond), &(s->lock));
    }
    s->value--;
    pthread_cleanup_pop(0);
    pthread_mutex_unlock(&(s->lock));
    return 0;
}

int sem_post(m_sem_t *s)
{
     pthread_mutex_lock(&(s->lock));
     s->value++;
     pthread_cond_signal(&(s->cond));
     pthread_mutex_unlock(&(s->lock));
    return 0;
}
