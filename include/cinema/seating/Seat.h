#pragma once

#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"
#include "cinema/seating/SeatTypes.h"

#include <string>

namespace cinema {

// Lop co so TRUU TUONG, mo ta ghe VAT LY (khong chua trang thai dat cho).
class Seat {
public:
    virtual ~Seat() = default;                 // ham huy ao: xoa dung lop con qua con tro Seat*

    Seat(const Seat&) = delete;                // chan copy lop da hinh (tranh slicing)
    Seat& operator=(const Seat&) = delete;

    const SeatId& id() const;
    virtual SeatType type() const = 0;
    virtual Money price() const = 0;
    virtual int capacity() const = 0;
    virtual std::string describe() const = 0;  // vi du "A5 [VIP] 120,000 VND"

protected:
    explicit Seat(SeatId id);                  // chi lop con moi tao duoc

private:
    SeatId id_;
};

}  // namespace cinema
