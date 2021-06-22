#ifndef FITS_HDU_H
#define FITS_HDU_H
#include <optional>
#include "fits/header.h"

namespace fits {

struct HDU {
    Header header;
    std::optional<std::streampos> address;
    virtual ~HDU() {};
    size_t size() {return 0;};
};

}
#endif /* ifndef FITS_HDU_H */
