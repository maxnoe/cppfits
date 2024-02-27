#include "catch2/catch.hpp"
#include "fits/file.h"
#include "fits/imagehdu.h"
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
    REQUIRE(fits.loaded_hdus() == 0);
    REQUIRE(fits.has_next_hdu());

    auto& hdu = fits.read_next_hdu();
    REQUIRE(fits.loaded_hdus() == 1);

    // first HDU must be an ImageHDU
    fits::ImageHDU* primary_hdu = dynamic_cast<fits::ImageHDU*>(&hdu);
    REQUIRE(primary_hdu != nullptr);

    REQUIRE(primary_hdu->address() == 0);

    // this file contains an empty primary HDU
    REQUIRE(primary_hdu->data_size() == 0);

    // this file contains a single header block
    REQUIRE(primary_hdu->byte_size() == BLOCK_SIZE);

    // no more hdus in the file
    REQUIRE(!fits.has_next_hdu());
}

TEST_CASE("FITS open simple image primary") {
    fits::FITS fits("tests/data/int16_image.fits");
    const auto& hdu = fits.read_next_hdu();

    // first HDU must be an ImageHDU
    const fits::ImageHDU* primary_hdu = dynamic_cast<const fits::ImageHDU*>(&hdu);
    REQUIRE(primary_hdu != nullptr);


    // this file contains an image of 3x4 uint16_t
    REQUIRE(primary_hdu->header().get<int64_t>("NAXIS") == 2);
    REQUIRE(primary_hdu->header().get<int64_t>("NAXIS1") == 3);
    REQUIRE(primary_hdu->header().get<int64_t>("NAXIS2") == 4);
    REQUIRE(primary_hdu->header().get<int64_t>("BITPIX") == 16);

    // real data size is  12 * 2 bytes, but FITS is always padded to BLOCK_SIZE
    REQUIRE(primary_hdu->payload_size() == 24);
    REQUIRE(primary_hdu->data_size() == BLOCK_SIZE);

    // this file contains a single header block and the data also fits in one block
    REQUIRE(primary_hdu->byte_size() == 2 * BLOCK_SIZE);

    REQUIRE_THROWS_WITH(fits.read_next_hdu(), "No more hdus in the file");
}

TEST_CASE("FITS open image extension") {
    fits::FITS fits("tests/data/image_extension.fits");

    // skip first
    REQUIRE(fits.has_next_hdu());
    fits.read_next_hdu();

    // We expect to find an image hdu
    REQUIRE(fits.has_next_hdu());
    const auto& hdu = fits.read_next_hdu();
    REQUIRE(fits.loaded_hdus() == 2);
    
    auto img_hdu = dynamic_cast<const fits::ImageHDU*>(&hdu);
    REQUIRE(img_hdu != nullptr);

    // this file contains an image of 3x4 uint16_t
    REQUIRE(img_hdu->header().get<int64_t>("NAXIS") == 2);
    REQUIRE(img_hdu->header().get<int64_t>("NAXIS1") == 3);
    REQUIRE(img_hdu->header().get<int64_t>("NAXIS2") == 4);
    REQUIRE(img_hdu->header().get<int64_t>("BITPIX") == 32);

    // real data size is  12 * 4 bytes, but FITS is always padded to BLOCK_SIZE
    REQUIRE(img_hdu->payload_size() == 48);
    REQUIRE(img_hdu->data_size() == BLOCK_SIZE);

    // this file contains a single header block and the data also fits in one block
    REQUIRE(img_hdu->byte_size() == 2 * BLOCK_SIZE);
    REQUIRE_THROWS_WITH(fits.read_next_hdu(), "No more hdus in the file");
}
