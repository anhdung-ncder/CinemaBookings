#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "cinema/catalog/Movie.h"
#include "cinema/catalog/Showtime.h"
#include "cinema/seating/Room.h"

namespace cinema {
	class Cinema {
	public:
		Cinema() = default;

		void addMovie(std::shared_ptr<const Movie> movie);
		void addRoom(std::shared_ptr<const Room> room);
		void addShowtime(std::shared_ptr<Showtime> showtime);

		std::shared_ptr<const Movie> movie(const std::string& id) const;
		std::shared_ptr<const Room> room(const std::string& id) const;
		std::shared_ptr<Showtime> showtime(const std::string& id) const;

		std::vector<std::shared_ptr<const Movie>> movies() const;
		std::vector<std::shared_ptr<const Room>> rooms() const;
		std::vector<std::shared_ptr<Showtime>> showtimes() const;

	private:
		std::map<std::string, std::shared_ptr<const Movie>> movies_;
		std::map<std::string, std::shared_ptr<const Room>> rooms_;
		std::map<std::string, std::shared_ptr<Showtime>> showtimes_;

	};
}