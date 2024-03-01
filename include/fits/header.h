#ifndef FITS_HEADER_H
#define FITS_HEADER_H
#include <iostream>
#include <string_view>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

#include "fits/constants.h"
#include "fits/utils.h"


inline std::ostream& operator << (std::ostream& oss, std::monostate state) {
    oss << "<No-Value>";
    return oss;
}

namespace fits {


struct HeaderEntry {
    using no_value = std::monostate;
    using value_t = std::variant<no_value, std::string, bool, int64_t, double>;

    std::string key;
    value_t value = no_value{};
    std::string comment;

    HeaderEntry() = default;
    template<typename T>
    HeaderEntry(std::string key, T value, std::string comment=""):
        key(std::move(key)),
        value(std::move(value)),
        comment(std::move(comment))
    {};

    static HeaderEntry parse(std::string_view line);

    bool has_value() const {
        return value.index() != 0 && !value.valueless_by_exception();
    }
};


struct Header {
    static Header read_from(std::istream& stream);

    std::streamsize byte_size() const {
        return add_padding(entries_.size() * ENTRY_SIZE);
    }

    template<typename T>
    T get(const std::string& key) const {
        return std::get<T>(entries_by_key_.at(key).value);
    }

    template<typename T>
    T get(const std::string& key, T default_value) const {
        // no such key
        if (entries_by_key_.count(key) == 0) {
            return default_value;
        }

        auto value = entries_by_key_.at(key).value;
        if (!std::holds_alternative<T>(value)) {
            return default_value;
        }
        return std::get<T>(value);
    }

    bool has_key(const std::string& key) const {
        return entries_by_key_.count(key) != 0;
    }

    const std::vector<HeaderEntry>& entries() const {
        return entries_;
    }

    const HeaderEntry& operator[](const std::string& key) const {
        return entries_by_key_.at(key);
    }

    const HeaderEntry& operator[](const size_t idx) {
        return entries_.at(idx);
    }

    private:
        std::vector<HeaderEntry> entries_;
        std::unordered_map<std::string, HeaderEntry> entries_by_key_;
};

} // namespace fits
#endif /* ifndef FITS_HEADER_H */
