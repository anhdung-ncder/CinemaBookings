#pragma once
#include <string>
#include "cinema/common/CinemaError.h"

namespace cinema {
	class ShowtimeClosedError : public CinemaError {		//Chặn đặt vé khi suất chiếu đã đóng
	public:
		explicit ShowtimeClosedError(const std::string& showtimeId)
			: CinemaError("Showtime closed: " + showtimeId), showtimeId_(showtimeId){ }
		const std::string& showtimeId() const {
			return showtimeId_;
		}
	private: 
		std::string showtimeId_;
	};

	class NotFoundError : public CinemaError {		//Xử lí tra cứu không thấy
	public:
		explicit NotFoundError(const std::string& message)
			: CinemaError(message) { }
	};

	class FileError : public CinemaError {		//Báo lỗi khi nạp file dữ liệu CSV bị hỏng
	public:
		explicit FileError(const std::string& message)
			: CinemaError(message) { }
	};
}
