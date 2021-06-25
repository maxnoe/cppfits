#include <string>
#include <iostream>
#include <variant>
#include "fits/file.h"

int main (int argc, char* argv[]) {
    std::string path;
    if (argc == 1) {
        path = "./tests/data/empty_primary_only.fits";
    } else {
        path = argv[1];
    }
    fits::FITS fits(path);

    const fits::HDU& hdu = *fits.hdus.back().get();

    std::cout << std::boolalpha;
    for (const auto& entry: hdu.header.lines) {
        std::cout << entry.key << ": ";
        std::visit([](auto v){std::cout << v;}, entry.value);
        if (entry.comment.size() > 0) {
            std::cout << " / " << entry.comment;
        }
        std::cout << '\n';
    }

    return 0;
}
