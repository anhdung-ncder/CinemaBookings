#pragma once

#include <exception>
#include <iostream>

inline int g_failed_tests = 0;

// Kiem tra dieu kien dung/sai
#define CHECK(condition)                                                      \
    do {                                                                      \
        if (!(condition)) {                                                   \
            std::cerr << "[FAILED] " << #condition                            \
                      << " at " << __FILE__ << ":" << __LINE__ << "\n";       \
            ++g_failed_tests;                                                 \
        }                                                                     \
    } while (false)

// Kiem tra co nem dung loai Exception mong doi
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

// Kiem tra bieu thuc chay khong nem bat ky exception nao
#define CHECK_NOTHROW(expression)                                            \
    do {                                                                      \
        try {                                                                 \
            expression;                                                       \
        } catch (const std::exception& e) {                                   \
            std::cerr << "[FAILED] " << #expression                           \
                      << " unexpected exception: " << e.what()               \
                      << " at " << __FILE__ << ":" << __LINE__ << "\n";       \
            ++g_failed_tests;                                                 \
        } catch (...) {                                                       \
            std::cerr << "[FAILED] " << #expression                           \
                      << " unexpected unknown exception"                     \
                      << " at " << __FILE__ << ":" << __LINE__ << "\n";       \
            ++g_failed_tests;                                                 \
        }                                                                     \
    } while (false)

// Tong ket ca test: tra ve 0 neu moi ca deu pass, tra ve 1 neu co loi
inline int check_summary() {
    if (g_failed_tests == 0) {
        std::cout << "\nAll checks passed!\n";
        return 0;
    }
    std::cerr << "\n" << g_failed_tests << " check(s) failed!\n";
    return 1;
}
