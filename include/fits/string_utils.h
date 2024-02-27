#ifndef FITS_STRING_UTILS_H
#define FITS_STRING_UTILS_H

#include <string_view>

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

} // namespace fits

#endif // !FITS_STRING_UTILS_H
