#ifndef FITS_FILE_H
#define FITS_FILE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>

namespace fits {

class HDU;
class ImageHDU;

class FITS {
    public:
        FITS() = default;
        FITS(const std::string& filename);

        HDU& read_next_hdu();
        bool has_next_hdu();
        void open(const std::string& filename);

        HDU& operator[](const size_t idx);
        HDU& operator[](const std::string& idx);

        size_t loaded_hdus() {return hdus_.size();}

        friend class HDU;
        friend class ImageHDU;

    private:
        std::ifstream stream_;
        std::vector<std::unique_ptr<HDU>> hdus_;
        std::unordered_map<std::string, HDU*> hdus_by_extname_;

        std::streampos address_of_next_hdu();
        template<typename T>
        T read(const std::streamsize n);

};


template<>
std::string FITS::read<std::string>(const std::streamsize n);


}
#endif /* ifndef FITS_FILE_H */
