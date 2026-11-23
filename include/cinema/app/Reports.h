#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "cinema/booking/Booking.h"
#include "cinema/booking/BookingService.h"
#include "cinema/catalog/Cinema.h"
#include "cinema/catalog/Showtime.h"
#include "cinema/common/Money.h"

namespace cinema {
	class Reports {
	public:

		static std::vector<std::shared_ptr<Showtime>> showtimesByDate(
			const Cinema& cinema, const std::string& dateStr);

		static std::vector<std::shared_ptr<Showtime>> availableShowtime(
			const Cinema& cinema);

		static std::vector<std::shared_ptr<const Movie>> searchMovies(
			const Cinema& cinema, const std::string& keyword);

		static std::map<std::string, Money> revenueByMovie(
			const BookingService& service);

		static std::vector<std::shared_ptr<const Booking>> bookingsByCustomer(
			const BookingService& service, const std::string& customer);
	};

}