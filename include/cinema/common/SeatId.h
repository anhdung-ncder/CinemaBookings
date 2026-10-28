#pragma once

#include <iosfwd>
#include <string>

namespace cinema {

class SeatId {
public:
    SeatId(char row, int number);
    static SeatId parse(const std::string& text);

    char row() const;
    int number() const;
    std::string toString() const;

private:
    char row_;
    int number_;
};

bool operator==(const SeatId& lhs, const SeatId& rhs);
bool operator!=(const SeatId& lhs, const SeatId& rhs);
bool operator<(const SeatId& lhs, const SeatId& rhs);
std::ostream& operator<<(std::ostream& os, const SeatId& id);

}  // namespace cinema
