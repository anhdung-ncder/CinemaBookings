
#pragma once

#include <string>

#include "cinema/common/CinemaError.h"

namespace cinema {

    // Thanh toan that bai: charge/refund khong thanh cong
    // (khong du tien, vuot han muc, hoan tien loi...)
    class PaymentFailedError : public CinemaError {
    public:
        explicit PaymentFailedError(const std::string& message)
            : CinemaError(message) {
        }
    };

    // Loi nghiep vu cua dat ve: dau vao sai, id khong ton tai,
    // huy lan hai, ma thanh toan la...
    class BookingError : public CinemaError {
    public:
        explicit BookingError(const std::string& message)
            : CinemaError(message) {
        }
    };

}  