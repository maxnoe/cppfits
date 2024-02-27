#include "fits/imagehdu.h"

#include <fmt/core.h>
#include <fmt/format.h>

namespace fits {



std::ostream& operator << (std::ostream& oss, const ImageType type) {
    oss << fmt::format("{}", type);
    return oss;
}

std::streamsize ImageHDU::payload_size() const {
    const size_t n_axis = naxis();
    if (n_axis == 0) {
        return 0;
    }

    std::streamsize size = 1;
    for (int axis=1; axis <= n_axis; axis++) {
        size *= naxis(axis);
    }

    // negative BITPIX means floating, must be ignored here
    size *= llabs(header_.get<int64_t>("BITPIX")) / 8;
    return size;
}

size_t ImageHDU::naxis() const {
    return header_.get<int64_t>("NAXIS");
}

size_t  ImageHDU::naxis(size_t axis) const {
    return header_.get<int64_t>(fmt::format("NAXIS{}", axis));
}

ImageType ImageHDU::type() const {
    return ImageType{header_.get<int64_t>("BITPIX")};
}

size_t ImageHDU::size() const {
    if (naxis() == 0) {
        return 0;
    }
    size_t s = 1;
    for (int axis=1; axis <= naxis(); axis++) {
        s *= naxis(axis);
    }
    return s;
}

std::vector<size_t> ImageHDU::shape() const {
    const auto n_axis = naxis();
    std::vector<size_t> shp;
    shp.reserve(n_axis);
    for (int axis=1; axis <= naxis(); axis++) {
        shp.push_back(naxis(axis));
    }
    return shp;
}

} // namespace fits
