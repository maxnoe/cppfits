#ifndef FITS_FILE_H
#define FITS_FILE_H value
#include <string>
#include <string_view>
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <optional>
#include <variant>
#include <fstream>
#include <memory>

#include "fits/hdu.h"
#include "fits/header.h"
#include "fits/constants.h"

namespace fits {

struct FITSException : public std::exception {
    std::string msg;
    FITSException() = default;
    FITSException(std::string msg) : msg(std::move(msg)) {};
    virtual const char* what() const noexcept override {
        return msg.c_str();
    }
};




class FITS {
    private:
        std::ifstream stream;

        template<typename T>
        T read(const std::streamsize n);

    public:
        FITS(const std::string& filename);
        std::vector<std::unique_ptr<HDU>> hdus;

        std::unique_ptr<HDU> read_next_hdu();
        void open(const std::string& filename) {
            stream.open(filename, std::ios::in | std::ios::binary);
            stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        }
};

template<>
std::string FITS::read<std::string>(const std::streamsize n){
    std::string s(n, '\0');
    stream.read(&s.front(), n);
    s.resize(stream.gcount());
    return s;
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

std::unique_ptr<HDU> FITS::read_next_hdu() {
    size_t next_hdu = hdus.size() == 0 ? 0 : hdus.back()->size();
    stream.seekg(next_hdu);
    bool end_found = false;
    char block[BLOCK_SIZE];

    auto hdu = std::make_unique<HDU>();

    while (!end_found) {
        stream.read(block, BLOCK_SIZE);
        auto read = stream.gcount();
        if (read < BLOCK_SIZE) {
            throw FITSException("Premature EOF");
        }


        for (size_t i=0; i < N_ENTRIES_BLOCK; i++) {
            std::string_view line(&block[i * ENTRY_SIZE], ENTRY_SIZE);
            hdu->header.lines.push_back(HeaderEntry::parse(line));
            if (hdu->header.lines.back().key == "END") {
                end_found = true;
                break;
            }
        }
    }
    return hdu;
}

}
#endif /* ifndef FITS_FILE_H */
