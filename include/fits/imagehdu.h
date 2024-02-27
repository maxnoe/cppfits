#ifndef FITS_IMAGEHDU_H
#define FITS_IMAGEHDU_H


#include <cstdlib>

#include <fmt/core.h>
#include "boost/endian/conversion.hpp"
#include "xtensor/xarray.hpp"

#include "fits/imagetype.h"
#include "fits/hdu.h"
#include "fits/exceptions.h"


namespace fits {

class ImageHDU : public HDU {
public:
    using HDU::HDU;

    std::streamsize payload_size() const override;
    size_t naxis() const;
    size_t naxis(size_t axis) const;
    std::vector<size_t> shape() const;
    ImageType type() const;
    size_t size() const;

    template<class T>
    xt::xarray<T> read() const {
        ImageType requested = get_image_type<T>();
        if (requested != type()) {
            auto msg = fmt::format("Image has type {} not {}", type(), requested);
            throw WrongImageType(msg);
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

}

#endif // !FITS_IMAGEHDU_H
