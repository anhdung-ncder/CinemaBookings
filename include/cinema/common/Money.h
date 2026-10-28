
#pragma once

#include <iosfwd>   
#include <string>

namespace cinema {

    class Money {
    public:
        Money();                                  // 0 dong
        explicit Money(long long dong);           // dong < 0 -> std::invalid_argument

        long long dong() const;
        Money scaledPercent(int percent) const;   // dong * percent / 100, chia nguyen
        std::string toString() const;             // "120,000 VND"

        Money& operator+=(const Money& rhs);
        Money& operator-=(const Money& rhs);      // ket qua am -> std::invalid_argument

    private:
        long long dong_ = 0;                      // invariant: luon >= 0
    };

    
    Money operator+(Money lhs, const Money& rhs);
    Money operator-(Money lhs, const Money& rhs);
    Money operator*(const Money& m, int times);   // times >= 0

    bool operator==(const Money& a, const Money& b);
    bool operator!=(const Money& a, const Money& b);
    bool operator<(const Money& a, const Money& b);
    bool operator<=(const Money& a, const Money& b);
    bool operator>(const Money& a, const Money& b);
    bool operator>=(const Money& a, const Money& b);

    std::ostream& operator<<(std::ostream& os, const Money& m);

}  