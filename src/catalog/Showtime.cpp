#include "cinema/catalog/Showtime.h"
#include "cinema/catalog/CatalogErrors.h"

#include <stdexcept>
#include <utility>

namespace cinema {
	Showtime::Showtime(std::string id, std::shared_ptr<const Movie> movie, std::shared_ptr<const Room> room, DateTime start, bool open) :
		id_(std::move(id)), movie_(std::move(movie)), room_(std::move(room)), start_(start), open_(open), seatMap_(room_) {
		if (id_.empty()) throw std::invalid_argument("Showtime: id must not be empty");
		if (!movie_) throw std::invalid_argument("Showtime: movie must not be null");
		if (!room_) throw std::invalid_argument("Showtime: room must not be null");
	}
	const std::string& Showtime::id() const {
		return id_;
	}
	const Movie& Showtime::movie() const {
		return *movie_;
	}
		const Room& Showtime::room() const {
			return *room_;
		}
		const DateTime& Showtime::start() const {
			return start_;
		}
		bool Showtime::isOpen() const {
			return open_;
		}
		void Showtime::open() {
			open_ = true;
		}
		void Showtime::close() {
			open_ = false;
		}
		const SeatMap& Showtime::seatMap() const {
			return seatMap_;
		}

	void Showtime::holdSeats(const std::vector<SeatId>& ids) {
		if (!open_) {
			throw ShowtimeClosedError(id_);
		}
		seatMap_.hold(ids);
	}
	void Showtime::confirmSeats(const std::vector<SeatId>& ids) {
		seatMap_.confirm(ids);
	}
	void Showtime::releaseSeats(const std::vector<SeatId>& ids) {
		seatMap_.release(ids);
	}
	void Showtime::restoreSoldSeats(const std::vector<SeatId>& ids) {
		seatMap_.restoreSold(ids);
	}
	Money Showtime::quote(const std::vector<SeatId>& ids) const {
		return seatMap_.quote(ids);
	}
}
