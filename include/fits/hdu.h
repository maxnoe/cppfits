#ifndef FITS_HDU_H
#define FITS_HDU_H
#include <cstdlib>
#include <cmath>

#include "boost/endian/conversion.hpp"
#include "xtensor/xarray.hpp"

#include "fits/header.h"
#include "fits/file.h"

namespace fits {


struct BaseHDU {
    Header header;
    FITS* file = nullptr;
    std::streampos address = 0;
    BaseHDU() {};
    BaseHDU(std::streampos address, FITS& file) : address(address), file(&file) {};

    virtual std::streamsize byte_size() const {
        return header.byte_size() + data_size();
    };
    virtual std::streamsize payload_size() const = 0;
    virtual std::streamsize data_size() const {
        return add_padding(payload_size());
    };
    virtual std::streampos data_address() const {
        return address + header.byte_size();
    }

    protected:
        ~BaseHDU() {};
};


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


struct ImageHDU : public BaseHDU {
    using BaseHDU::BaseHDU;

    std::streamsize payload_size() const override;
    size_t naxis() const;
    size_t naxis(size_t axis) const;
    ImageType type() const;
    size_t size() const;

    std::vector<size_t> shape() const;

    template<class T>
    xt::xarray<T> read() {
        ImageType requested = get_image_type<T>();
        if (requested != type()) {
            std::stringstream ss;
            ss << "Image has type " << type() << " not " << requested;
            throw std::runtime_error(ss.str());
        };
        auto img = xt::empty<T>(shape());
        auto buffer = img.data();
        file->stream.seekg(data_address());
        for (size_t idx=0; idx < size(); idx++) {
            file->stream.read(reinterpret_cast<char*>(&buffer[idx]), sizeof(T));
            boost::endian::big_to_native_inplace(buffer[idx]);
        }
        return img;
    }
};


using HDU = std::variant<ImageHDU>;

}
#endif /* ifndef FITS_HDU_H */
