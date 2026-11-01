#include "cinema/catalog/Cinema.h"
#include "cinema/catalog/CatalogErrors.h"
#include <algorithm>
#include<stdexcept>

namespace cinema {
	void Cinema::addMovie(std::shared_ptr<const Movie> movie) {
		if (!movie) throw std::invalid_argument("Cinema: movie must not be null");
		movies_[movie->id()] = movie;
	}
	void Cinema::addRoom(std::shared_ptr<const Room> room) {
		if (!room) throw std::invalid_argument("Cinema: room must not be null");
		rooms_[room->id()] = room;
	}

	void Cinema::addShowtime(std::shared_ptr<Showtime> showtime) {
		if (!showtime) throw std::invalid_argument("Cinema: showtime must not be null");
		showtimes_[showtime->id()] = showtime;
	}

	std::shared_ptr<const Movie> Cinema::movie(const std::string& id) const {
		auto it = movies_.find(id);
		if (it == movies_.end()) {
			throw NotFoundError("Movie not found: " + id);
		}
		return it->second;
	}

	std::shared_ptr<const Room> Cinema::room(const std::string& id) const {
		auto it = rooms_.find(id);
		if (it == rooms_.end()) {
			throw NotFoundError("Room not found: " + id);
		}
		return it->second;
	}

	std::shared_ptr<Showtime> Cinema::showtime(const std::string& id) const {
		auto it = showtimes_.find(id);
		if (it == showtimes_.end()) {
			throw NotFoundError("Showtime not found: " + id);
		}
		return it->second;
	}

	std::vector<std::shared_ptr<const Movie>> Cinema::movies() const {
		std::vector<std::shared_ptr<const Movie>> result;
		for (const auto& pair : movies_) {
			result.push_back(pair.second);
		}
		return result;
	}

	std::vector<std::shared_ptr<const Room>> Cinema::rooms() const {
		std::vector<std::shared_ptr<const Room>> result;
		for (const auto& pair : rooms_) {
			result.push_back(pair.second);
		}
		return result;
	}

	std::vector<std::shared_ptr<Showtime>> Cinema::showtimes() const {
		std::vector<std::shared_ptr<Showtime>> result;
		for (const auto& pair : showtimes_) {
			result.push_back(pair.second);
		}
		std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
			return a->start() < b->start();
			});
		return result;
	}
}