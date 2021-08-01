#include "fits/file.h"
#include "fits/hdu.h"

namespace fits {

template<>
std::string FITS::read<std::string>(const std::streamsize n){
    std::string s(n, '\0');
    stream.read(&s.front(), n);
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
    const auto get_next_address = [](const auto& hdu) {
        return hdu.address + hdu.byte_size();
    };
    return std::visit(get_next_address, hdus.back());
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

    hdus.push_back(ImageHDU(next_address, *this));
    ImageHDU& hdu = std::get<ImageHDU>(hdus.back());

    while (!end_found) {
        stream.read(block, BLOCK_SIZE);
        auto read = stream.gcount();
        if (read < BLOCK_SIZE) {
            throw FITSException("Premature EOF");
        }

        for (size_t i=0; i < N_ENTRIES_BLOCK; i++) {
            std::string_view line(&block[i * ENTRY_SIZE], ENTRY_SIZE);
            hdu.header.lines.push_back(HeaderEntry::parse(line));
            HeaderEntry& entry = hdu.header.lines.back();

            // add entries with values to the hash map for easy lookup
            if (entry.has_value()) {
                hdu.header.vals[entry.key] = entry;
            }

            if (hdu.header.lines.back().key == "END") {
                end_found = true;
                break;
            }
        }
    }
    return hdus.back();
}

}
