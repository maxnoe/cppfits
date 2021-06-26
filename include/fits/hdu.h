#ifndef FITS_HDU_H
#define FITS_HDU_H
#include <cstdlib>
#include <cmath>
#include "fits/header.h"

namespace fits {


struct BaseHDU {
    Header header;
    std::streampos address;
    BaseHDU(std::streampos address) : address(address) {};

    virtual std::streamsize size() const {
        return header.byte_size() + data_size();
    };
    virtual std::streamsize data_size() const = 0;

    protected:
        ~BaseHDU() {};
};


struct ImageHDU : public BaseHDU {
    using BaseHDU::BaseHDU;

    std::streamsize payload_size() const {
        int64_t naxis = header.get<int64_t>("NAXIS");
        if (naxis == 0) {
            return 0;
        }

        std::streamsize size = 1;
        for (int axis=1; axis <= naxis; axis++) {
            size *= header.get<int64_t>("NAXIS" + std::to_string(axis));
        }

        // negative BITPIX means floating, must be ignored here
        size *= llabs(header.get<int64_t>("BITPIX")) / 8;
        return size;
    }

    std::streamsize data_size() const {
        return add_padding(payload_size());
    }
};


using HDU = std::variant<ImageHDU>;

}
#endif /* ifndef FITS_HDU_H */
