#ifndef FITS_HEADER_H
#define FITS_HEADER_H
#include <string_view>
#include <string>
#include <variant>
#include <vector>


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
    using value_t = std::variant<std::string, bool, int64_t, double>;
    std::string key;
    value_t value = "";
    std::string comment;

    HeaderEntry() = default;
    template<typename T>
    HeaderEntry(std::string key, T value, std::string comment=""):
        key(std::move(key)),
        value(std::move(value)),
        comment(std::move(comment))
    {};

    static HeaderEntry parse(std::string_view line);
};


struct Header {
    std::vector<HeaderEntry> lines;
};

} // namespace fits
#endif /* ifndef FITS_HEADER_H */
