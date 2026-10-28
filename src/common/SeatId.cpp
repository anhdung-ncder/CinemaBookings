#include "cinema/common/SeatId.h"

#include "cinema/seating/SeatErrors.h"

#include <algorithm>
#include <cctype>
#include <ostream>

namespace cinema {

namespace {
constexpr char kMinRow = 'A';
constexpr char kMaxRow = 'Z';
constexpr int kMinNumber = 1;
constexpr int kMaxNumber = 99;
}  // namespace

SeatId::SeatId(char row, int number) : row_(row), number_(number) {
    if (row < kMinRow || row > kMaxRow || number < kMinNumber || number > kMaxNumber) {
        throw SeatNotFoundError("Invalid seat id: row=" + std::string(1, row) +
                                ", number=" + std::to_string(number));
    }
}

SeatId SeatId::parse(const std::string& text) {
    if (text.size() < 2 || text.size() > 3) {
        throw SeatNotFoundError("Invalid seat id text: '" + text + "'");
    }

    const char row = static_cast<char>(std::toupper(static_cast<unsigned char>(text[0])));

    const std::string digits = text.substr(1);
    const bool allDigits = std::all_of(digits.begin(), digits.end(), [](char c) {
        return std::isdigit(static_cast<unsigned char>(c)) != 0;
    });
    if (!allDigits) {
        throw SeatNotFoundError("Invalid seat id text: '" + text + "'");
    }

    return SeatId(row, std::stoi(digits));
}

char SeatId::row() const { return row_; }

int SeatId::number() const { return number_; }

std::string SeatId::toString() const { return std::string(1, row_) + std::to_string(number_); }

bool operator==(const SeatId& lhs, const SeatId& rhs) {
    return lhs.row() == rhs.row() && lhs.number() == rhs.number();
}

bool operator!=(const SeatId& lhs, const SeatId& rhs) { return !(lhs == rhs); }

bool operator<(const SeatId& lhs, const SeatId& rhs) {
    if (lhs.row() != rhs.row()) {
        return lhs.row() < rhs.row();
    }
    return lhs.number() < rhs.number();
}

std::ostream& operator<<(std::ostream& os, const SeatId& id) { return os << id.toString(); }

}  // namespace cinema
