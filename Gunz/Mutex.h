#ifndef MUTEX_H_
#define MUTEX_H_

class Mutex
{
public:
	
	Mutex()
	{
		InitializeCriticalSection(&_mutex);
	}

	void Aquire()
	{
		EnterCriticalSection(&_mutex);
	}

	void Release()
	{
		LeaveCriticalSection(&_mutex);
	}
	
private:
	CRITICAL_SECTION _mutex;
};

#endif