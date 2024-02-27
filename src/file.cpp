#include "fits/file.h"

#include <memory>
#include <fmt/core.h>

#include "fits/hdu.h"
#include "fits/exceptions.h"
#include "fits/imagehdu.h"

namespace fits {

template<>
std::string FITS::read<std::string>(const std::streamsize n){
    std::string s(n, '\0');
    stream.read(s.data(), n);
    s.resize(stream.gcount());
    return s;
}

void FITS::open(const std::string& filename) {
    stream.open(filename, std::ios::in | std::ios::binary);
    HeaderEntry marker;
    try {
        marker = HeaderEntry::parse(read<std::string>(ENTRY_SIZE));
        if (marker.key != "SIMPLE" || !std::get<bool>(marker.value)) {
            throw FITSException("Not a FITS file");
        }
    } catch (...) {
        throw FITSException("Not a FITS file");
    }
    stream.seekg(0);
}

FITS::FITS(const std::string& filename){
    open(filename);
}

std::streampos FITS::address_of_next_hdu() {
    if (hdus.empty()) {
        return 0;
    }
    return hdus.back()->address_of_next_hdu();
}

bool FITS::has_next_hdu() {
    std::streampos next_address = address_of_next_hdu();
    stream.seekg(next_address);
    stream.peek();
    return stream.good();
}

HDU& FITS::read_next_hdu() {
    if (!has_next_hdu()) {
        throw FITSException("No more hdus in the file");
    }

    size_t next_address = address_of_next_hdu();
    bool end_found = false;
    char block[BLOCK_SIZE];

    auto hdu = std::make_unique<ImageHDU>(next_address, *this);

    while (!end_found) {
        stream.read(block, BLOCK_SIZE);
        auto read = stream.gcount();
        if (read < BLOCK_SIZE) {
            throw FITSException("Premature EOF");
        }

        for (size_t i=0; i < N_ENTRIES_BLOCK; i++) {
            std::string_view line(&block[i * ENTRY_SIZE], ENTRY_SIZE);
            hdu->header_.lines.push_back(HeaderEntry::parse(line));
            HeaderEntry& entry = hdu->header_.lines.back();

            // add entries with values to the hash map for easy lookup
            if (entry.has_value()) {
                hdu->header_.vals[entry.key] = entry;
            }

            if (hdu->header_.lines.back().key == "END") {
                end_found = true;
                break;
            }
        }
    }
    hdus.push_back(std::move(hdu));
    return *hdus.back();
}

HDU& FITS::operator[](const size_t idx) {
    // already loaded, just return
    if (idx < hdus.size()) {
        return *hdus[idx];
    }

    // check if more HDUs are available
    while (has_next_hdu()) {
        read_next_hdu();
        if (idx < hdus.size()) {
            return *hdus[idx];
        }
    }

    auto msg = fmt::format(
        "HDU index {} is out of range for FITS file with {} hdus.",
        idx,
        hdus.size()
    );
    throw NoSuchHDU(msg);
}

}
