#include "cinema/seating/SeatFactory.h"

#include "cinema/seating/StandardSeat.h"
#include "cinema/seating/VipSeat.h"
#include "cinema/seating/CoupleSeat.h"
#include "cinema/seating/SeatErrors.h"

#include <string>

namespace cinema {

    std::unique_ptr<Seat> SeatFactory::create(char typeCode,const SeatId& id,const Money& standardPrice ) {
        switch (typeCode) {
        case 'S':
            return std::make_unique<StandardSeat>(id, standardPrice);

        case 'V':
            return std::make_unique<VipSeat>(id, standardPrice);

        case 'C':
            return std::make_unique<CoupleSeat>(id, standardPrice);

        default:
            throw InvalidLayoutError(
                std::string("Invalid seat type code: ") + typeCode
            );
        }
    }

} 