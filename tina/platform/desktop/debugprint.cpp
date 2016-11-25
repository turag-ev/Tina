#ifdef QT_CORE_LIB

#include <cstdio>
#include <cstdarg>
#include <QDebug>
#include <QString>

extern "C"
void turag_debug_vprintf(const char* fmt, va_list vargs) {
    QString buffer;
    buffer.vsprintf(fmt, vargs);
    va_end(vargs);

    qDebug().noquote() << buffer.mid(3, buffer.length() - 4).toLocal8Bit();
}

extern "C"
void turag_debug_puts(const char* s)
{
    QString out(s);
    qDebug().noquote() << out.mid(3, out.length() - 4).toLocal8Bit();
}


#endif // #ifdef QT_CORE_LIB
