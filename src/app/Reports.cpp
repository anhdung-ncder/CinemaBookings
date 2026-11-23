#include "cinema/app/Reports.h"
#include "cinema/seating/SeatTypes.h"

#include <algorithm>
#include <cctype>
#include <iterator>
#include <numeric>

namespace cinema {
	static std::string toLowerStr(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c));});
		return s;
	}


	std::vector<std::shared_ptr<Showtime>> Reports::showtimesByDate(
		const Cinema& cinema, const std::string& dateStr) {
		const auto all = cinema.showtimes();
		std::vector<std::shared_ptr<Showtime>> result;
		std::copy_if(all.begin(), all.end(), std::back_inserter(result),
			[&dateStr](const std::shared_ptr<Showtime>& st) {
				return st->start().dateString() == dateStr;
			});
		return result;
	}

	std::vector<std::shared_ptr<Showtime>> Reports::availableShowtime(
		const Cinema& cinema) {
		const auto all = cinema.showtimes();
		std::vector<std::shared_ptr<Showtime>> result;
		std::copy_if(all.begin(), all.end(), std::back_inserter(result),
			[](const std::shared_ptr<Showtime>& st) {
				return st->isOpen() && st->seatMap().count(SeatState::Available) > 0;
			});
		return result;
	}

	std::vector<std::shared_ptr<const Movie>> Reports::searchMovies(
		const Cinema& cinema, const std::string& keyword) {
		const auto all = cinema.movies();
		const std::string keyLower = toLowerStr(keyword);
		std::vector<std::shared_ptr<const Movie>> result;
		std::copy_if(all.begin(), all.end(), std::back_inserter(result),
			[&keyLower](const std::shared_ptr<const Movie>& m) {
				return toLowerStr(m->title()).find(keyLower) != std::string::npos;
			});
		return result;
	}

	std::map<std::string, Money> Reports::revenueByMovie(
		const BookingService& service) {
		std::map<std::string, Money> revenue;
		for (const auto& booking : service.all()) {
			if (booking->status() == BookingStatus::Confirmed) {
				const std::string& movieId = booking->showtime().movie().id();
				revenue[movieId] += booking->total();
			}
		}
		return revenue;
	}

	std::vector<std::shared_ptr<const Booking>> Reports::bookingsByCustomer(
		const BookingService& service, const std::string& customer) {
		const auto all = service.all();
		std::vector<std::shared_ptr<const Booking>> result;
		std::copy_if(all.begin(), all.end(), std::back_inserter(result),
			[&customer](const std::shared_ptr<const Booking>& b) {
				return b->customer() == customer;
			});
		return result;
	}

}