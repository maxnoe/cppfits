#include "fits/hdu.h"

namespace fits {

std::ostream& operator << (std::ostream& oss, const ImageType type) {
    switch (type) {
        case ImageType::FLOAT:  oss << "FLOAT"; break;
        case ImageType::DOUBLE: oss << "DOUBLE"; break;
        case ImageType::UINT8:  oss << "UINT8"; break;
        case ImageType::INT16:  oss << "INT16"; break;
        case ImageType::INT32:  oss << "INT32"; break;
        case ImageType::INT64:  oss << "INT64"; break;
        default: oss << "INVALID";
    }

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
    size *= llabs(header.get<int64_t>("BITPIX")) / 8;
    return size;
}

size_t ImageHDU::naxis() const {
    return header.get<int64_t>("NAXIS");
}

size_t  ImageHDU::naxis(size_t axis) const {
    return header.get<int64_t>("NAXIS" + std::to_string(axis));
}

ImageType ImageHDU::type() const {
    return ImageType{header.get<int64_t>("BITPIX")};
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

}
