#pragma once
#include <string>
#include <iosfwd>

namespace cinema {
	class DateTime {
		public:
			DateTime(int year, int month, int day, int hour, int minute);

		static DateTime parse(const std::string& text);
		
			std::string toString() const;
			std::string dateString() const;

			int year() const;
			int month() const;
			int day() const;
			int hour() const;
			int minute() const;
		private:
			int year_ = 2000;
			int month_ = 1;
			int day_ = 1;
			int hour_ = 0;
			int minute_ = 0;
	};
	bool operator==(const DateTime& a, const DateTime& b);
	bool operator<(const DateTime& a, const DateTime& b);
	std::ostream& operator<<(std::ostream& os, const DateTime& dt);
}