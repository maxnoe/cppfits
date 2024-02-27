#ifndef FITS_IMAGETYPE_H
#define FITS_IMAGETYPE_H

#include <fmt/core.h>
#include <type_traits>
#include <cstdint>
#include <ostream>

namespace fits {

enum class ImageType : int64_t {
    UINT8 = 8,
    INT16 = 16,
    INT32 = 32,
    INT64 = 64,
    FLOAT = -32,
    DOUBLE = -64,
};

std::ostream& operator << (std::ostream& oss, const ImageType type);

template<typename T>
constexpr ImageType get_image_type() {
    if constexpr (std::is_same_v<T, float>) {
        return ImageType::FLOAT;
    } else if constexpr (std::is_same_v<T, double>) {
        return ImageType::DOUBLE;
    } else if constexpr (std::is_same_v<T, uint8_t>) {
        return ImageType::UINT8;
    } else if constexpr (std::is_same_v<T, int16_t>) {
        return ImageType::INT16;
    } else if constexpr (std::is_same_v<T, int32_t>) {
        return ImageType::INT32;
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return ImageType::INT64;
    } else {
        throw std::runtime_error("Invalid type for FITS Image");
    }
}

} // namespace fits

template<> struct fmt::formatter<fits::ImageType> : formatter<string_view> {

    auto format(fits::ImageType type, format_context& ctx) const {
        string_view name = "INVALID";
        switch (type) {
            case fits::ImageType::FLOAT:  name = "FLOAT"; break;
            case fits::ImageType::DOUBLE: name = "DOUBLE"; break;
            case fits::ImageType::UINT8:  name = "UINT8"; break;
            case fits::ImageType::INT16:  name = "INT16"; break;
            case fits::ImageType::INT32:  name = "INT32"; break;
            case fits::ImageType::INT64:  name = "INT64"; break;
        }
        return formatter<string_view>::format(name, ctx);
    }
};

#endif // !FITS_IMAGETYPE_H
