#include <string>
#include <iostream>
#include <variant>
#include "fits/file.h"


std::ostream& operator << (std::ostream& oss, std::monostate state) {
    std::cout << "<No-Value>" << std::endl;
    return oss;
}

int main (int argc, char* argv[]) {
    std::string path;
    if (argc == 1) {
        path = "./tests/data/empty_primary_only.fits";
    } else {
        path = argv[1];
    }
    fits::FITS fits(path);

    const auto& hdu = std::get<fits::ImageHDU>(fits.hdus.back());

    std::cout << std::boolalpha;
    for (const auto& entry: hdu.header.lines) {
        std::cout << entry.key;
        std::visit([](auto v){std::cout << "= " << v;}, entry.value);
        if (entry.comment.size() > 0) {
            if (entry.key != "COMMENT" && entry.key != "HISTORY") {
                std::cout << " / ";
            }
            std::cout << entry.comment;
        }
        std::cout << '\n';
    }

    return 0;
}
