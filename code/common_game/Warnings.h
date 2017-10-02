#pragma once

#ifdef _MSC_VER

#define WARNINGS_OFF(...) __pragma (warning(push))\
                          __pragma (warning(disable: __VA_ARGS__))

#define WARNINGS_ON __pragma (warning(pop))

#endif
