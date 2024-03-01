#include "fits/bintablehdu.h"
#include "fits/exceptions.h"
#include "fits/string_utils.h"
#include <cstdint>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <ios>
#include <regex>
#include <string>

namespace fits {


const std::regex tform_regex{R"(([0-9]*)([LXABIJKEDCMPQ])(.*))"};


std::vector<size_t> parse_tdim(const std::string& tdim) {
    std::stringstream tdim_stream(std::string{strip(tdim)});
    std::vector<size_t> shape;
    if (tdim_stream.peek() != '(') {
        throw FITSException(fmt::format("Invalid TDIM: '{}'", tdim));
    }
    tdim_stream.ignore();
    for (int n_elements; tdim_stream >> n_elements; ) {
        shape.push_back(n_elements);
        if (tdim_stream.peek() == ',') {
            tdim_stream.ignore();
        } else if (tdim_stream.peek() == ')') {
            break;
        }
    }

    return shape;
}


ColumnDescription get_column_description(Header& header, size_t index) {
    ColumnDescription desc;
    desc.index = index;
    // mandatory attributes
    std::string tform{strip(header.get<std::string>(fmt::format("TFORM{}", index)))};
    std::smatch match;
    if (!std::regex_match(tform, match, tform_regex)) {
        throw FITSException(fmt::format("TFORM{} is invalid: '{}'", index, tform));
    }

    desc.type = ColumnType(match[2].str()[0]);
    desc.size = match[1] == "" ? 1ull : std::stoull(match[1]);

    if (header.has_key(fmt::format("TDIM{}", index))) {
        std::string tdim = header.get<std::string>(fmt::format("TDIM{}", index));
        try {
            desc.shape = parse_tdim(tdim);
        } catch (...) {
            throw FITSException(fmt::format("Invalid TDIM{}: '{}'", index, tdim));
        }
    }

    // optional attributes
    desc.name = strip(header.get<std::string>(fmt::format("TTYPE{}", index), fmt::format("_unnamed_column{}", index)));
    desc.unit = strip(header.get<std::string>(fmt::format("TUNIT{}", index), ""));

    return desc;
}

BinTableHDU::BinTableHDU(std::streampos address, Header&& header, FITS& file) :
    HDU(address, std::move(header), file)
{
    for (size_t index = 1; index <= n_columns(); index++) {
        column_descriptions_.push_back(get_column_description(header_, index));
    }
}


std::streamsize BinTableHDU::payload_size() const {
    std::streamsize n_bytes_per_row = header_.get<int64_t>("NAXIS1");
    std::streamsize n_rows = header_.get<int64_t>("NAXIS2");
    return n_bytes_per_row * n_rows;
}

};
