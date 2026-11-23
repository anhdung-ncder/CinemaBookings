#pragma once
#include <string>
#include "cinema/app/DataStore.h"
#include "cinema/booking/BookingService.h"
#include "cinema/catalog/Cinema.h"
#include "cinema/catalog/Showtime.h"

namespace cinema {
	class Menu {
	public:
		Menu(Cinema& cinema, BookingService& service, DataStore& store);

		void run();

	private:
		Cinema& cinema_;
		BookingService& service_;
		DataStore& store_;

		void handleListShowtimesAndSeats() const;
		void handleBookingTickets();
		void handleCancelBooking();
		void handleReports() const;
		void handleStorage();
		void handleToggleShowtime();

		static std::string readline(const std::string& prompt);
		static int readInt(const std::string& prompt);
		static void printSeatMap(const Showtime& showtime);
	};
}