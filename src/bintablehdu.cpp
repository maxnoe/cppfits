#include "fits/bintablehdu.h"
#include <cstdint>
#include <ios>

namespace fits {

std::streamsize BinTableHDU::payload_size() const {
    std::streamsize n_bytes_per_row = header_.get<int64_t>("NAXIS1");
    std::streamsize n_rows = header_.get<int64_t>("NAXIS2");
    return n_bytes_per_row * n_rows;
}

};
