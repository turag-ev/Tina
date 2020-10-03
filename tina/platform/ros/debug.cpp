#include <tina/debug/defines.h>
#include <tina/debug/print.h>
#include <ros/console.h>
#include <memory>

namespace TURAG {

//
// Tina Schnittstelle

extern "C"
void turag_debug_puts(const char* s) {
    const char* msg = &s[3];
    const char prefix = s[2];
    const unsigned char source = s[1];
    const char* sourcename = turag_log_source_table[source];
    if (!sourcename)
        sourcename = "TinA";
    switch(prefix) {
    case TURAG_DEBUG_DEBUG_PREFIX[0]:
        ROS_DEBUG_NAMED(sourcename, "%s", msg);
        break;
    case TURAG_DEBUG_INFO_PREFIX[0]:
        ROS_INFO_NAMED(sourcename, "%s", msg);
        break;
    case TURAG_DEBUG_WARN_PREFIX[0]:
        ROS_WARN_NAMED(sourcename, "%s", msg);
        break;
    case TURAG_DEBUG_CRITICAL_PREFIX[0]:
        ROS_ERROR_NAMED(sourcename, "%s", msg);
        break;
    case TURAG_DEBUG_ERROR_PREFIX[0]:
        ROS_ERROR_NAMED(sourcename, "%s", msg);
        break;
    }
}

static std::unique_ptr<char> dyn_vprintf(const char* fmt, va_list vargs) {
    va_list bkup_vargs;
    va_copy(bkup_vargs, vargs); //first vsnprintf call modifies vargs
    size_t size = vsnprintf( nullptr, 0, fmt, vargs) + 1; // Extra space for '\0'
    std::unique_ptr<char> buffer(new char[size]);
    vsnprintf(buffer.get(), size, fmt, bkup_vargs);
    va_end(bkup_vargs);
    return buffer;
}
    
extern "C"
void turag_debug_vprintf(const char* fmt, va_list vargs) {
    const char* format = &fmt[3]; //strip prefix
    const char prefix = fmt[2];
    const char unsigned source = fmt[1];
    const char* sourcename = turag_log_source_table[source];
    if (!sourcename)
        sourcename = "TinA";
    switch(prefix) {
    case TURAG_DEBUG_DEBUG_PREFIX[0]:
        ROS_DEBUG_NAMED(sourcename, "%s", dyn_vprintf(format,vargs).get() );
        break;
    case TURAG_DEBUG_INFO_PREFIX[0]:
        ROS_INFO_NAMED(sourcename, "%s", dyn_vprintf(format,vargs).get() );
        break;
    case TURAG_DEBUG_WARN_PREFIX[0]:
        ROS_WARN_NAMED(sourcename, "%s", dyn_vprintf(format,vargs).get() );
        break;
    case TURAG_DEBUG_CRITICAL_PREFIX[0]:
        ROS_ERROR_NAMED(sourcename, "%s", dyn_vprintf(format,vargs).get() );
        break;
    case TURAG_DEBUG_ERROR_PREFIX[0]:
        ROS_ERROR_NAMED(sourcename, "%s", dyn_vprintf(format,vargs).get() );
        break;
    }
}

} // namespace TURAG
