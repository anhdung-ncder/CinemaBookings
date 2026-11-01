#pragma once
#include <string>

namespace cinema {
	class Movie {
	public:
		Movie(std::string id, std::string title, int durationMinutes, std::string genre);

		const std::string& id() const;
		const std::string& title() const;
		int durationMinutes() const;
		const std::string& genre() const;

	private:
		std::string id_;
		std::string title_;
		int durationMinutes_ = 0;
		std::string genre_;
	};
}
