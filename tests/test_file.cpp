#include "catch2/catch.hpp"
#include "fits/file.h"
#include "fits/hdu.h"
#include "fits/constants.h"

TEST_CASE("FITS open invalid") {
    fits::FITS fits;
    REQUIRE_THROWS_WITH(
        fits.open("tests/data/invalid.fits"),
        "Not a FITS file"
    );
}

TEST_CASE("FITS open empty primary") {
    fits::FITS fits("tests/data/empty.fits");
    REQUIRE(fits.hdus.size() == 0);
    REQUIRE(fits.has_next_hdu());

    auto& hdu = fits.read_next_hdu();
    REQUIRE(fits.hdus.size() == 1);

    // first HDU must be an ImageHDU
    REQUIRE(std::holds_alternative<fits::ImageHDU>(hdu));

    const auto& primary_hdu = std::get<fits::ImageHDU>(hdu);
    REQUIRE(primary_hdu.address == 0);

    // this file contains an empty primary HDU
    REQUIRE(primary_hdu.data_size() == 0);

    // this file contains a single header block
    REQUIRE(primary_hdu.byte_size() == BLOCK_SIZE);

    // no more hdus in the file
    REQUIRE(!fits.has_next_hdu());
}

TEST_CASE("FITS open simple image primary") {
    fits::FITS fits("tests/data/int16_image.fits");
    const auto& hdu = fits.read_next_hdu();

    // first HDU must be an ImageHDU
    REQUIRE(std::holds_alternative<fits::ImageHDU>(hdu));

    const auto& img_hdu = std::get<fits::ImageHDU>(hdu);

    // this file contains an image of 3x4 uint16_t
    REQUIRE(img_hdu.header.get<int64_t>("NAXIS") == 2);
    REQUIRE(img_hdu.header.get<int64_t>("NAXIS1") == 3);
    REQUIRE(img_hdu.header.get<int64_t>("NAXIS2") == 4);
    REQUIRE(img_hdu.header.get<int64_t>("BITPIX") == 16);

    // real data size is  12 * 2 bytes, but FITS is always padded to BLOCK_SIZE
    REQUIRE(img_hdu.payload_size() == 24);
    REQUIRE(img_hdu.data_size() == BLOCK_SIZE);

    // this file contains a single header block and the data also fits in one block
    REQUIRE(img_hdu.byte_size() == 2 * BLOCK_SIZE);

    REQUIRE_THROWS_WITH(fits.read_next_hdu(), "No more hdus in the file");
}

TEST_CASE("FITS open image extension") {
    fits::FITS fits("tests/data/image_extension.fits");

    // skip first
    fits.read_next_hdu();
    REQUIRE(fits.has_next_hdu());


    // We expect to find an image hdu
    const auto& hdu = fits.read_next_hdu();
    REQUIRE(fits.hdus.size() == 2);

    REQUIRE(std::holds_alternative<fits::ImageHDU>(hdu));
    const auto& img_hdu = std::get<fits::ImageHDU>(hdu);

    // this file contains an image of 3x4 uint16_t
    REQUIRE(img_hdu.header.get<int64_t>("NAXIS") == 2);
    REQUIRE(img_hdu.header.get<int64_t>("NAXIS1") == 3);
    REQUIRE(img_hdu.header.get<int64_t>("NAXIS2") == 4);
    REQUIRE(img_hdu.header.get<int64_t>("BITPIX") == 32);

    // real data size is  12 * 4 bytes, but FITS is always padded to BLOCK_SIZE
    REQUIRE(img_hdu.payload_size() == 48);
    REQUIRE(img_hdu.data_size() == BLOCK_SIZE);

    // this file contains a single header block and the data also fits in one block
    REQUIRE(img_hdu.byte_size() == 2 * BLOCK_SIZE);
    REQUIRE_THROWS_WITH(fits.read_next_hdu(), "No more hdus in the file");
}
