#pragma once

#include "cinema/common/CinemaError.h"
#include "cinema/common/SeatId.h"

#include <string>

namespace cinema {

class SeatTakenError : public CinemaError {
public:
    explicit SeatTakenError(const SeatId& seatId)
        : CinemaError("Seat already taken: " + seatId.toString()), seatId_(seatId) {}

    const SeatId& seatId() const { return seatId_; }

private:
    SeatId seatId_;
};

class SeatNotFoundError : public CinemaError {
public:
    explicit SeatNotFoundError(const std::string& message) : CinemaError(message) {}
};

class InvalidSeatStateError : public CinemaError {
public:
    explicit InvalidSeatStateError(const std::string& message) : CinemaError(message) {}
};

class InvalidLayoutError : public CinemaError {
public:
    explicit InvalidLayoutError(const std::string& message) : CinemaError(message) {}
};

}  // namespace cinema
