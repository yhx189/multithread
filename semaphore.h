#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

 typedef struct m_sem_t {
	int value;
        pthread_mutex_t lock;
        pthread_cond_t cond;
 } m_sem_t;
int sem_post(m_sem_t *s);
int sem_wait(m_sem_t *s);
void sem_init(m_sem_t *s, int value);
#endif
