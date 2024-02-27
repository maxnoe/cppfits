#include "fits/file.h"

#include <istream>
#include <memory>
#include <fmt/core.h>

#include "fits/bintablehdu.h"
#include "fits/hdu.h"
#include "fits/exceptions.h"
#include "fits/imagehdu.h"

namespace fits {

template<>
std::string FITS::read<std::string>(const std::streamsize n){
    std::string s(n, '\0');
    stream_.read(s.data(), n);
    s.resize(stream_.gcount());
    return s;
}

void FITS::open(const std::string& filename) {
    stream_.open(filename, std::ios::in | std::ios::binary);
    HeaderEntry marker;
    try {
        marker = HeaderEntry::parse(read<std::string>(ENTRY_SIZE));
        if (marker.key != "SIMPLE" || !std::get<bool>(marker.value)) {
            throw FITSException("Not a FITS file");
        }
    } catch (...) {
        throw FITSException("Not a FITS file");
    }
    stream_.seekg(0);
}

FITS::FITS(const std::string& filename){
    open(filename);
}

std::streampos FITS::address_of_next_hdu() {
    if (hdus_.empty()) {
        return 0;
    }
    return hdus_.back()->address_of_next_hdu();
}

bool FITS::has_next_hdu() {
    std::streampos next_address = address_of_next_hdu();
    stream_.seekg(next_address);
    stream_.peek();
    return stream_.good();
}

HDU& FITS::read_next_hdu() {
    if (!has_next_hdu()) {
        throw FITSException("No more hdus in the file");
    }

    size_t next_address = address_of_next_hdu();

    std::unique_ptr<HDU> hdu;
    Header header = Header::read_from(stream_);

    // First is always an ImageHDU
    if (loaded_hdus() == 0) {
        hdu = std::make_unique<ImageHDU>(next_address, header, *this);
    } else {
        // for all xtensions, check xtension header
        if (!header.has_key("XTENSION")) {
            throw FITSException("No XTENSION header in HDU");
        }

        auto xtension = strip(header.get<std::string>("XTENSION"));
        if (xtension == "IMAGE") {
            hdu = std::make_unique<ImageHDU>(next_address, header, *this);
        } else if (xtension == "BINTABLE") {
            hdu = std::make_unique<BinTableHDU>(next_address, header, *this);
        } else {
            throw std::runtime_error(fmt::format("Unknown xtension: '{}'", xtension));
        }
    }
    hdus_.push_back(std::move(hdu));
    return *hdus_.back();
}


HDU& FITS::operator[](const size_t idx) {
    // already loaded, just return
    if (idx < hdus_.size()) {
        return *hdus_[idx];
    }

    // check if more HDUs are available
    while (has_next_hdu()) {
        read_next_hdu();
        if (idx < hdus_.size()) {
            return *hdus_[idx];
        }
    }

    auto msg = fmt::format(
        "HDU index {} is out of range for FITS file with {} hdus.",
        idx,
        hdus_.size()
    );
    throw NoSuchHDU(msg);
}

}
