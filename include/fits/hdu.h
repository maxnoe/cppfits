#ifndef FITS_HDU_H
#define FITS_HDU_H

#include "fits/header.h"
#include "fits/file.h"
#include <utility>

namespace fits {


class HDU {

public:
    HDU() {}
    HDU(std::streampos address, Header&& header, FITS& file) :
        address_(address),
        header_(header),
        file_(&file) {}

    virtual ~HDU() {}

    virtual std::streamsize byte_size() const {
        return header_.byte_size() + data_size();
    };

    virtual std::streamsize payload_size() const = 0;

    virtual std::streamsize data_size() const {
        return add_padding(payload_size());
    };

    virtual std::streampos data_address() const {
        return address_ + header_.byte_size();
    }

    virtual std::streampos address_of_next_hdu() const {
        return address_ + byte_size();
    }

    const Header& header() const { return header_; }
    size_t address() const { return address_; }

protected:
    Header header_;
    FITS* file_ = nullptr;
    std::streampos address_ = 0;

    friend class FITS;

};

}
#endif /* ifndef FITS_HDU_H */
