#pragma once
#include <iostream>

inline int g_failed_tests = 0;

#define CHECK(condition)                                                      \
    do {                                                                      \
        if (!(condition)) {                                                   \
            std::cerr << "[FAILED] " << #condition                            \
                      << " at " << __FILE__ << ":" << __LINE__ << "\n";       \
            ++g_failed_tests;                                                 \
        }                                                                     \
    } while (false)


#define CHECK_THROWS_AS(expression, ExceptionType)                            \
    do {                                                                      \
        bool caught = false;                                                  \
        try {                                                                 \
            expression;                                                       \
        } catch (const ExceptionType&) {                                      \
            caught = true;                                                    \
        } catch (...) {                                                       \
            caught = false;                                                   \
        }                                                                     \
        if (!caught) {                                                        \
            std::cerr << "[FAILED] " << #expression                           \
                      << " expected exception: " << #ExceptionType            \
                      << " at " << __FILE__ << ":" << __LINE__ << "\n";       \
            ++g_failed_tests;                                                 \
        }                                                                     \
    } while (false)
