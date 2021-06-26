#ifndef FITS_HDU_H
#define FITS_HDU_H
#include <optional>
#include <cstdint>
#include "fits/header.h"

namespace fits {


struct BaseHDU {
    Header header;
    std::optional<std::streampos> address;
    BaseHDU(std::streampos address) : address(address) {};
    virtual ~BaseHDU() {};

    virtual size_t size() const {
        return header.byte_size() + data_size();
    };
    virtual size_t data_size() const = 0;
};


struct ImageHDU : public BaseHDU {
    using BaseHDU::BaseHDU;
    size_t data_size() const override {
        return 0;
    }
};


using HDU = std::variant<ImageHDU>;

}
#endif /* ifndef FITS_HDU_H */
