#ifndef LOCKER_H
#define LOCKER_H

#include <stdio.h>
#include <exception>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>

class sem
{
public:
    sem()
    {
        if( sem_init( &m_sem, 0, 0 ) != 0 )
        {
            throw std::exception();
        }
    }
    ~sem()
    {
        sem_destroy( &m_sem );
    }
    bool wait()
    {
        return sem_wait( &m_sem ) == 0;
    }
    bool try_wait()
    {
        return sem_trywait( &m_sem ) == 0;
    }
    bool post()
    {
        return sem_post( &m_sem ) == 0;
    }

private:
    sem_t m_sem;
};

class locker
{
public:
    locker()
    {
        if( pthread_mutex_init( &m_mutex, NULL ) != 0 )
        {
            throw std::exception();
        }
    }
    ~locker()
    {
        pthread_mutex_destroy( &m_mutex );
    }
    bool lock()
    {
        return pthread_mutex_lock( &m_mutex ) == 0;
    }
    bool trylock()
    {
        return pthread_mutex_trylock( &m_mutex ) == 0;
    }
    bool unlock()
    {
        return pthread_mutex_unlock( &m_mutex ) == 0;
    }

private:
    pthread_mutex_t m_mutex;
};

class cond
{
public:
    cond()
    {
        if( pthread_mutex_init( &m_mutex, NULL ) != 0 )
        {
            throw std::exception();
        }
        if ( pthread_cond_init( &m_cond, NULL ) != 0 )
        {
            pthread_mutex_destroy( &m_mutex );
            throw std::exception();
        }
    }
    ~cond()
    {
        pthread_mutex_destroy( &m_mutex );
        pthread_cond_destroy( &m_cond );
    }
    bool wait()
    {
        int ret = 0;
        pthread_mutex_lock( &m_mutex );
        ret = pthread_cond_wait( &m_cond, &m_mutex );
        pthread_mutex_unlock( &m_mutex );
        return ret == 0;
    }
    int timedwait(struct timespec ts)
    {
        int ret = 0;
        pthread_mutex_lock( &m_mutex );
        ret = pthread_cond_timedwait( &m_cond, &m_mutex, &ts);
        pthread_mutex_unlock( &m_mutex );
        if (ret == 0) {
            return 0;
        //else if (ret == ETIMEDOUT) 
        } else  {
            return -1;
        }
    }
    bool signal()
    {
        return pthread_cond_signal( &m_cond ) == 0;
    }

private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

#endif