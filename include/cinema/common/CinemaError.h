#pragma once
#include <string>
#include <stdexcept>
namespace cinema {
	class CinemaError : public std::runtime_error {
	public:
			explicit CinemaError(const std::string& message)
				:std::runtime_error(message) {}
	};
}