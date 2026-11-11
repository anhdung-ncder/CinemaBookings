#pragma once
#include <string>
#include <memory>
#include "cinema/catalog/Cinema.h"
#include "cinema/booking/BookingService.h"

namespace cinema {
	class DataStore {
	public:
		explicit DataStore(std::string dataDir = "data");

		void load(Cinema& cinema, BookingService& service);

		void save(const Cinema& cinema, const BookingService& service);

		const std::string& dataDir() const { return dataDir_; }


	private:
		std::string dataDir_;

		void loadMovies(const std::string& filePath, Cinema& cinema);
		void loadRooms(const std::string& filePath, Cinema& cinema);
		void loadShowtimes(const std::string& filePath, Cinema& cinema);
		void loadBookings(const std::string& filePath, Cinema& cinema, BookingService& service);
		
		void saveMovies(const std::string& filePath, const Cinema& cinema);
		void saveRooms(const std::string& filePath, const Cinema& cinema);
		void saveShowtimes(const std::string& filePath, const Cinema& cinema);
		void saveBookings(const std::string& filePath, const BookingService& service);
	};
}
