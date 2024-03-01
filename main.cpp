#include <string>
#include <iostream>
#include <iomanip>
#include <variant>
#include "fits/file.h"
#include "fits/imagehdu.h"

#include "xtensor/xio.hpp"


void print_header(const fits::Header& header) {
    for (const auto& entry: header.entries()) {
        std::cout << std::left << std::setw(8) << entry.key;

        if (entry.has_value()) {
            std::visit([](auto v){std::cout << "= " << v;}, entry.value);
        }

        if (entry.comment.size() > 0) {
            if (entry.has_value()) {
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
        path = "./tests/data/empty.fits";
    } else {
        path = argv[1];
    }

    std::cout << std::boolalpha;

    fits::FITS fits(path);
    while (fits.has_next_hdu()) {
        const auto& hdu = fits.read_next_hdu();
        print_header(hdu.header());
        std::cout << "\n\n";

        if (auto image_hdu = dynamic_cast<const fits::ImageHDU*>(&hdu)) {
            if (image_hdu->type() == fits::ImageType::INT16) {
                std::cout << image_hdu->read<int16_t>() << "\n";
            } else if (image_hdu->type() == fits::ImageType::INT32) {
                std::cout << image_hdu->read<int32_t>() << "\n";
            } else if (image_hdu->type() == fits::ImageType::FLOAT) {
                std::cout << image_hdu->read<float>() << "\n";
            }
        }
    }
    return 0;
}
