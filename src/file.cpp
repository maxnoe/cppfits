#include "fits/file.h"

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
    stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
}


FITS::FITS(const std::string& filename){
    open(filename);
    auto marker = read<std::string>(MARKER.size());
    if (marker != MARKER) {
        throw FITSException("Not a FITS file");
    }
    stream.seekg(0);
    hdus.push_back(read_next_hdu());
}

size_t get_size(BaseHDU& hdu) {
    return hdu.size();
}


HDU& FITS::read_next_hdu() {
    size_t next_hdu = hdus.size() == 0 ? 0 : std::visit(get_size, hdus.back());
    stream.seekg(next_hdu);
    bool end_found = false;
    char block[BLOCK_SIZE];

    hdus.push_back(ImageHDU(next_hdu));
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
