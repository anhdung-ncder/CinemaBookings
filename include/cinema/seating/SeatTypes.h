#pragma once

#include <string>

namespace cinema {

enum class SeatType { Standard, Vip, Couple };
enum class SeatState { Available, Held, Sold };

char toCode(SeatType type);             // 'S', 'V', 'C'
std::string toString(SeatType type);    // "Standard", "VIP", "Couple"
std::string toString(SeatState state);  // "Available", "Held", "Sold"

}  // namespace cinema
