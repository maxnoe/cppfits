#include <string>
#include <iostream>
#include <iomanip>
#include <variant>
#include "fits/file.h"
#include "fits/hdu.h"

#include "xtensor/xio.hpp"


std::ostream& operator << (std::ostream& oss, std::monostate state) {
    std::cout << "<No-Value>" << std::endl;
    return oss;
}

auto print_header = [](const auto& hdu) {
    for (const auto& entry: hdu.header.lines) {
        std::cout << std::left << std::setw(8) << entry.key;
        std::visit([](auto v){std::cout << "= " << v;}, entry.value);
        if (entry.comment.size() > 0) {
            if (entry.key != "COMMENT" && entry.key != "HISTORY") {
                std::cout << " / ";
            }
            std::cout << entry.comment;
        }
        std::cout << '\n';
    }
};


int main (int argc, char* argv[]) {
    std::string path;
    if (argc == 1) {
        path = "./tests/data/empty_primary_only.fits";
    } else {
        path = argv[1];
    }

    std::cout << std::boolalpha;

    fits::FITS fits(path);
    while (fits.has_next_hdu()) {
        const auto& hdu = fits.read_next_hdu();
        std::visit(print_header, hdu);
        std::cout << "\n\n";

        if (std::holds_alternative<fits::ImageHDU>(hdu)) {
            auto image_hdu = std::get<fits::ImageHDU>(hdu);
            std::cout << "NAXIS: " << image_hdu.naxis() << "\n";
            std::cout << "Type:  " << image_hdu.type() << "\n";

            if (image_hdu.type() == fits::ImageType::INT16) {
                std::cout << image_hdu.read<int16_t>() << "\n";
            } else if (image_hdu.type() == fits::ImageType::INT32) {
                std::cout << image_hdu.read<int32_t>() << "\n";
            } else if (image_hdu.type() == fits::ImageType::FLOAT) {
                std::cout << image_hdu.read<float>() << "\n";
            }
        }
    }
    return 0;
}
