#ifndef FITS_BINTABLEHDU_H
#define FITS_BINTABLEHDU_H


#include "fits/hdu.h"


namespace fits {

class BinTableHDU : public HDU {
public:
    using HDU::HDU;

    std::streamsize payload_size() const override;
};

} // namespace fits

#endif // !FITS_BINTABLEHDU_H
