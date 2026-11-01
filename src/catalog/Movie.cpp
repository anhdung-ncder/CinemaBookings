#include "cinema/catalog/Movie.h"

#include <stdexcept>
#include <utility>

namespace cinema {

    Movie::Movie(std::string id, std::string title, int durationMinutes, std::string genre)
        : id_(std::move(id)),
          title_(std::move(title)),
          durationMinutes_(durationMinutes),
          genre_(std::move(genre)) {
        if (id_.empty()) {
            throw std::invalid_argument("Movie: id must not be empty");
        }
        if (title_.empty()) {
            throw std::invalid_argument("Movie: title must not be empty");
        }
        if (durationMinutes_ < 1 || durationMinutes_ > 600) {
            throw std::invalid_argument("Movie: durationMinutes must be in range 1..600");
        }
    }

    const std::string& Movie::id() const {
        return id_;
    }

    const std::string& Movie::title() const {
        return title_;
    }

    int Movie::durationMinutes() const {
        return durationMinutes_;
    }

    const std::string& Movie::genre() const {
        return genre_;
    }

}
