#include <ch.h>
#if __cplusplus >= 201100L
#include <cstdint>
#endif
#include "stackusage.h"

#ifdef CH_DBG_FILL_THREADS

#if __cplusplus >= 201100L
std::size_t su_get_stack_usage(const char* stack, std::size_t stack_size) {
    // FIXME!
    
    /*const char* begin = stack + stack_size;
    const char* end = reinterpret_cast<char*>(reinterpret_cast<const thread_t*>(static_cast<const void*>(stack))->p_stklimit);
    for (const char* ptr = end; ptr != begin; ptr++) {
        if (*ptr != 0x55) {
            return (begin - ptr) * sizeof(char);
        }
    }
    */
    return 0;
}
#endif // __cplusplus >= 201100L

extern "C"
size_t su_get_stack_usageC(const char* stack, size_t stack_size) {
#if __cplusplus >= 201100L
    return su_get_stack_usage(stack, stack_size);
#else
    (void)stack;
    (void)stack_size;
    return 0;
#endif // __cplusplus >= 201100L
}

#endif // CH_DBG_FILL_THREADS
