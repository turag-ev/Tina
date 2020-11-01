#ifndef TESTS_EXTENTIONS_H
#define TESTS_EXTENTIONS_H

#include <iostream>

template<class C, class T>
std::basic_ostream<C, T>& operator<<(std::basic_ostream<C, T>& os, std::nullptr_t)
{
  return os << static_cast<void>(nullptr);
}

#endif // TESTS_EXTENTIONS_H
