#ifndef FITS_EXCEPTIONS_H
#define FITS_EXCEPTIONS_H
#include <stdexcept>

namespace fits {

class FITSException : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
};


class NoSuchHDU : public FITSException {
    public:
        using FITSException::FITSException;
};

class WrongImageType : public FITSException {
    public:
        using FITSException::FITSException;
};

}
#endif // !FITS_EXCEPTIONS_H
