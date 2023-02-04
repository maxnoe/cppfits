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


inline std::string_view left_strip(std::string_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(" "), s.size()));
    return s;
}

inline std::string_view right_strip(std::string_view s) {
    s.remove_suffix(std::min(s.size() - s.find_last_not_of(" ") - 1, s.size()));
    return s;
}

inline std::string_view strip(std::string_view s) {
    s = left_strip(s);
    s = right_strip(s);
    return s;
}

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
    friend class FITS;

    std::vector<HeaderEntry> lines;


    std::streamsize byte_size() const {
        return add_padding(lines.size() * ENTRY_SIZE);
    }

    template<typename T>
    T get(const std::string& key) const {
        return std::get<T>(vals.at(key).value);
    }

    private:
        std::unordered_map<std::string, HeaderEntry> vals;
};

} // namespace fits
#endif /* ifndef FITS_HEADER_H */
