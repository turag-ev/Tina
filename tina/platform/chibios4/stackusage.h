#ifndef STACKUSAGE_H
#define STACKUSAGE_H

#ifdef __cplusplus

#if __cplusplus >= 201100L
# include <cstdint>
std::size_t su_get_stack_usage(const char* stack, std::size_t stack_size);
#endif // __cplusplus >= 201100L

#else
# include <stdint.h>
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

size_t su_get_stack_usageC(const char* stack, size_t stack_size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
