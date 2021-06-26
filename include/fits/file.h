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
        FITS() = default;
        FITS(const std::string& filename);
        std::vector<HDU> hdus;

        HDU& read_next_hdu();
        bool has_next_hdu();
        std::streampos address_of_next_hdu();
        void open(const std::string& filename);
};


}
#endif /* ifndef FITS_FILE_H */
