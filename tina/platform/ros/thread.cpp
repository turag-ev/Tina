#include <algorithm>
#include <cstdint>
#include <vector>

#ifndef _WIN32
# include <sys/prctl.h>
#endif

#include <tina++/thread.h>

namespace TURAG {

namespace {

struct ThreadPrivate
{
  Mutex mutex;
  ConditionVariable var;
  bool paused;

  Mutex threads_mutex;
  std::vector<ThreadImpl*> threads;

  ThreadPrivate() :
	mutex(), var(&mutex), paused(false)
  { }

  // thread mgmt

  void addThread(ThreadImpl* thread)
  {
	  Mutex::Lock lock(threads_mutex);
	  threads.push_back(thread);
  }

  void rmThread(ThreadImpl* thread)
  {
	  Mutex::Lock lock(threads_mutex);
	  threads.erase(
		  std::remove(threads.begin(), threads.end(), thread), threads.end()
	  );
  }

  void terminateAllThreads()
  {
	  Mutex::Lock lock(threads_mutex);
	  for (auto* thread : threads)
	  {
		  thread->terminate();
	  }
  }

  void joinAllThreads()
  {
	  Mutex::Lock lock(threads_mutex);
	  for (auto* thread : threads)
	  {
		  thread->join();
	  }
  }
};

ThreadPrivate& getPrivate()
{
	static ThreadPrivate instance;
	return instance;
}

} // namespace


void CurrentThread::setName(const char *name)
{
#ifndef _WIN32
	prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(name), 0, 0, 0);
#endif
}

ThreadImpl::ThreadImpl() :
	terminate_request_(false)
{
	getPrivate().addThread(this);
}

ThreadImpl::~ThreadImpl()
{
	getPrivate().rmThread(this);
}

void ThreadImpl::join()
{
	if (thread_) {
		thread_->join();
	}
}

void ThreadImpl::start(int, void (*entry)())
{
	thread_.reset(new std::thread(entry));
}

void AllThreads::pause()
{
	auto& p = getPrivate();

	Mutex::Lock lock(p.mutex);
	p.paused = true;
}

void AllThreads::resume()
{
	auto& p = getPrivate();

	Mutex::Lock lock(p.mutex);
	p.paused = false;
	p.var.broadcast();
}

void AllThreads::join()
{
	getPrivate().joinAllThreads();
}

void AllThreads::terminate()
{
	getPrivate().terminateAllThreads();
}

} // namespace TURAG
