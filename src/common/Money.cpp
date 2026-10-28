
#include "cinema/common/Money.h"

#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <string>

namespace cinema {

    Money::Money() = default;   // dong_ da la 0 nho "= 0" o khai bao thanh vien

    Money::Money(long long dong) : dong_(dong) {
        if (dong < 0) {
            throw std::invalid_argument("Money: amount must not be negative");
        }
    }

    long long Money::dong() const { return dong_; }

    Money Money::scaledPercent(int percent) const {
        if (percent < 0) {
            throw std::invalid_argument("Money: percent must not be negative");
        }
        return Money(dong_ * percent / 100);   // chia nguyen -> cat phan le
    }

    std::string Money::toString() const {
        const std::string digits = std::to_string(dong_);
        std::string reversed;
        int count = 0;
        for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
            if (count > 0 && count % 3 == 0) {
                reversed.push_back(',');
            }
            reversed.push_back(*it);
            ++count;
        }
        std::reverse(reversed.begin(), reversed.end());
        return reversed + " VND";
    }

    Money& Money::operator+=(const Money& rhs) {
        dong_ += rhs.dong_;
        return *this;
    }

    Money& Money::operator-=(const Money& rhs) {
        if (rhs.dong_ > dong_) {                // kiem tra TRUOC khi sua -> that bai thi *this khong doi
            throw std::invalid_argument("Money: result must not be negative");
        }
        dong_ -= rhs.dong_;
        return *this;
    }

    Money operator+(Money lhs, const Money& rhs) {
        lhs += rhs;      // lhs la ban sao
        return lhs;
    }

    Money operator-(Money lhs, const Money& rhs) {
        lhs -= rhs;
        return lhs;
    }

    Money operator*(const Money& m, int times) {
        if (times < 0) {
            throw std::invalid_argument("Money: multiplier must not be negative");
        }
        return Money(m.dong() * times);
    }

    bool operator==(const Money& a, const Money& b) { return a.dong() == b.dong(); }
    bool operator!=(const Money& a, const Money& b) { return !(a == b); }
    bool operator<(const Money& a, const Money& b) { return a.dong() < b.dong(); }
    bool operator<=(const Money& a, const Money& b) { return !(b < a); }
    bool operator>(const Money& a, const Money& b) { return b < a; }
    bool operator>=(const Money& a, const Money& b) { return !(a < b); }

    std::ostream& operator<<(std::ostream& os, const Money& m) {
        return os << m.toString();
    }

} 