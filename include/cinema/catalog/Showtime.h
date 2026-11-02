#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cinema/catalog/DateTime.h"
#include "cinema/catalog/Movie.h"
#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"
#include "cinema/seating/Room.h"
#include "cinema/seating/SeatMap.h"

namespace cinema {

    class Showtime {
    public:
        Showtime(std::string id,
            std::shared_ptr<const Movie> movie,
            std::shared_ptr<const Room> room,
            DateTime start,
            bool open = true);

        const std::string& id() const;
        const Movie& movie() const;
        const Room& room() const;
        const DateTime& start() const;
        bool isOpen() const;
        void open();
        void close();

        const SeatMap& seatMap() const; // Chỉ đọc (để vẽ sơ đồ ghế)

        // 5 hàm Mặt tiền (Facade) cho tầng Booking:
        void holdSeats(const std::vector<SeatId>& ids);        // Suất đóng -> ném ShowtimeClosedError
        void confirmSeats(const std::vector<SeatId>& ids);     // Không kiểm tra đóng/mở
        void releaseSeats(const std::vector<SeatId>& ids);     // Không kiểm tra đóng/mở
        void restoreSoldSeats(const std::vector<SeatId>& ids); // Không kiểm tra đóng/mở
        Money quote(const std::vector<SeatId>& ids) const;     // Không kiểm tra đóng/mở

    private:
        std::string id_;
        std::shared_ptr<const Movie> movie_;
        std::shared_ptr<const Room> room_;
        DateTime start_;
        bool open_ = true;
        SeatMap seatMap_; // Mỗi suất chiếu sở hữu 1 SeatMap riêng
    };

}