#include "cinema/catalog/DateTime.h"

#include <cctype>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <tuple>

namespace cinema {

    namespace {
        // Kiem tra nam nhuan: chia het cho 4 nhung khong chia het cho 100, tru khi chia het cho 400
        bool isLeapYear(int year) {
            return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        }

        // Tra ve so ngay toi da cua thang trong nam
        int daysInMonth(int year, int month) {
            switch (month) {
                case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                    return 31;
                case 4: case 6: case 9: case 11:
                    return 30;
                case 2:
                    return isLeapYear(year) ? 29 : 28;
                default:
                    return 0;
            }
        }
    }

    DateTime::DateTime(int year, int month, int day, int hour, int minute)
        : year_(year), month_(month), day_(day), hour_(hour), minute_(minute) {
        if (year < 1 || year > 9999) {
            throw std::invalid_argument("DateTime: invalid year");
        }
        if (month < 1 || month > 12) {
            throw std::invalid_argument("DateTime: invalid month");
        }
        if (day < 1 || day > daysInMonth(year, month)) {
            throw std::invalid_argument("DateTime: invalid day for month/year");
        }
        if (hour < 0 || hour > 23) {
            throw std::invalid_argument("DateTime: invalid hour");
        }
        if (minute < 0 || minute > 59) {
            throw std::invalid_argument("DateTime: invalid minute");
        }
    }

    DateTime DateTime::parse(const std::string& text) {
        // Dinh dang bat buoc: "YYYY-MM-DD HH:MM" (dung 16 ky tu)
        if (text.size() != 16 ||
            text[4] != '-' || text[7] != '-' ||
            text[10] != ' ' || text[13] != ':') {
            throw std::invalid_argument("DateTime::parse: invalid format, expected YYYY-MM-DD HH:MM");
        }

        for (std::size_t i = 0; i < text.size(); ++i) {
            if (i == 4 || i == 7 || i == 10 || i == 13) {
                continue;
            }
            if (!std::isdigit(static_cast<unsigned char>(text[i]))) {
                throw std::invalid_argument("DateTime::parse: non-digit character in date/time");
            }
        }

        const int year   = std::stoi(text.substr(0, 4));
        const int month  = std::stoi(text.substr(5, 2));
        const int day    = std::stoi(text.substr(8, 2));
        const int hour   = std::stoi(text.substr(11, 2));
        const int minute = std::stoi(text.substr(14, 2));

        return DateTime(year, month, day, hour, minute);
    }

    std::string DateTime::dateString() const {
        std::ostringstream oss;
        oss << std::setfill('0')
            << std::setw(4) << year_ << '-'
            << std::setw(2) << month_ << '-'
            << std::setw(2) << day_;
        return oss.str();
    }

    std::string DateTime::toString() const {
        std::ostringstream oss;
        oss << dateString() << ' '
            << std::setfill('0')
            << std::setw(2) << hour_ << ':'
            << std::setw(2) << minute_;
        return oss.str();
    }

    int DateTime::year() const { return year_; }
    int DateTime::month() const { return month_; }
    int DateTime::day() const { return day_; }
    int DateTime::hour() const { return hour_; }
    int DateTime::minute() const { return minute_; }

    bool operator==(const DateTime& a, const DateTime& b) {
        return std::make_tuple(a.year(), a.month(), a.day(), a.hour(), a.minute()) ==
               std::make_tuple(b.year(), b.month(), b.day(), b.hour(), b.minute());
    }

    bool operator<(const DateTime& a, const DateTime& b) {
        return std::make_tuple(a.year(), a.month(), a.day(), a.hour(), a.minute()) <
               std::make_tuple(b.year(), b.month(), b.day(), b.hour(), b.minute());
    }

    std::ostream& operator<<(std::ostream& os, const DateTime& dt) {
        return os << dt.toString();
    }

}
