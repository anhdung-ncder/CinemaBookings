#pragma once

#include <iostream>

// Bien toan cuc inline (tinh nang C++17): dem so ca test that bai.
// Tu khoa "inline" giup nhieu file .cpp cung include check.h ma khong bi loi trung dinh nghia bien (ODR violation).
inline int g_failed_tests = 0;

// Macro CHECK: Kiem tra dieu kien dung/sai.
// Neu dieu kien false: in ra loi gom bieu thuc, ten file va so dong, roi tang bien dem.
#define CHECK(condition)                                                      \
    do {                                                                      \
        if (!(condition)) {                                                   \
            std::cerr << "[FAILED] " << #condition                            \
                      << " at " << __FILE__ << ":" << __LINE__ << "\n";       \
            ++g_failed_tests;                                                 \
        }                                                                     \
    } while (false)

// Macro CHECK_THROWS_AS: Kiem tra mot bieu thuc co nem dung loai exception khong.
// expression: cau lenh can kiem tra (vi du: Money(-100))
// ExceptionType: loai ngoai le mong doi (vi du: std::invalid_argument)
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
