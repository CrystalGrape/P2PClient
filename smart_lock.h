#ifndef smart_lock
#define smart_lock
#include <pthread.h>
static pthread_mutex_t buffer_lock;
class SmartLock
{
public:
	SmartLock(pthread_mutex_t *mutex)
	{
		pthread_mutex_lock(mutex);
		this->mutex = mutex;
	}
	~SmartLock()
	{
		pthread_mutex_unlock(mutex);
	}
private:
	pthread_mutex_t *mutex;
};
#endif
