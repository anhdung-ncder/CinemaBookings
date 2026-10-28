#include "cinema/seating/SeatTypes.h"

#include <stdexcept>

namespace cinema {

char toCode(SeatType type) {
    switch (type) {
        case SeatType::Standard: return 'S';
        case SeatType::Vip:      return 'V';
        case SeatType::Couple:   return 'C';
    }
    throw std::logic_error("unknown SeatType");
}

std::string toString(SeatType type) {
    switch (type) {
        case SeatType::Standard: return "Standard";
        case SeatType::Vip:      return "VIP";
        case SeatType::Couple:   return "Couple";
    }
    throw std::logic_error("unknown SeatType");
}

std::string toString(SeatState state) {
    switch (state) {
        case SeatState::Available: return "Available";
        case SeatState::Held:      return "Held";
        case SeatState::Sold:      return "Sold";
    }
    throw std::logic_error("unknown SeatState");
}

}  // namespace cinema
