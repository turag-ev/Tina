#ifdef _WIN32
# include <windows.h>
#else
# include <sys/time.h>
#endif

#include <cstdint>

extern "C" {

uint64_t turag_plattform_dependent_get_tick()
{
#ifdef _WIN32
		return GetTickCount(); 
#else
	  struct timeval time_value;
	  gettimeofday(&time_value, nullptr);
	  return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
#endif
}

} // extern "C"
