#ifndef FITS_BINTABLEHDU_H
#define FITS_BINTABLEHDU_H

#include <cstdint>
#include <fmt/core.h>
#include <optional>

#include "fits/hdu.h"

namespace fits {

enum class ColumnType : char {
    LOGICAL = 'L',
    BITARRAY = 'X',
    CHARACTER = 'A',
    U8 = 'B',
    I16 = 'I',
    I32 = 'J',
    I64 = 'K',
    F32 = 'E',
    F64 = 'D',
    C64 = 'C',
    C128 = 'M',
    VLARRAY32 = 'P',
    VLARRAY64 = 'Q',
};

struct ColumnDescription {
    size_t index;
    ColumnType type;
    size_t size;
    std::optional<std::vector<size_t>> shape;
    std::string name = "";
    std::string unit;
    double transform_offset = 0.;
    double transform_scale = 1.0;
    std::optional<int64_t> null_value;
    std::optional<std::string> display_format;
};

class BinTableHDU : public HDU {
public:
    using HDU::HDU;

    BinTableHDU(std::streampos address, Header&& header, FITS& file);

    std::streamsize payload_size() const override;

    int64_t n_columns() const {
        return header_.get<int64_t>("TFIELDS");
    }

    const std::vector<ColumnDescription>& column_descriptions() const {
        return column_descriptions_;
    }

private:
    std::vector<ColumnDescription> column_descriptions_;
};



} // namespace fits

#endif // !FITS_BINTABLEHDU_H
