#include "check.h"

#include "cinema/catalog/CatalogErrors.h"
#include "cinema/catalog/Cinema.h"
#include "cinema/catalog/DateTime.h"
#include "cinema/catalog/Movie.h"
using namespace cinema; 
	void testDateTime() {
		CHECK_NOTHROW(DateTime(2024, 2, 29, 18, 0));
		CHECK_THROWS_AS(DateTime(2023, 2, 29, 18, 0), std::invalid_argument);

		CHECK_THROWS_AS(DateTime(2024, 12, 1, 24, 0), std::invalid_argument);

		DateTime dt = DateTime::parse("2026-12-03 18:04");
		CHECK(dt.year() == 2026);
		CHECK(dt.hour() == 18);

	}

	void testMovie() {
		CHECK_THROWS_AS(Movie("M1", "Fast and Furious", 0, "Actions"), std::invalid_argument);
		CHECK_THROWS_AS(Movie("M1", "Fast and Furious", 601, "Actions"), std::invalid_argument);

		Movie m("M1", "Avatar", 180, "Sci-Fi");
		CHECK(m.title() == "Avatar");
	}


	void testCinema() {
		Cinema c;
		CHECK_THROWS_AS(c.movie("M_UNKNOWN"), NotFoundError);
	}

	int main() {
		testDateTime();
		testMovie();
		testCinema();

		return check_summary();		//in ra "all checks passed" neu kh co loi
	}