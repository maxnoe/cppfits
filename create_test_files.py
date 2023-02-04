from astropy.io import fits
import numpy as np
from pathlib import Path


FILES = []

OUTPATH = Path('tests/data')


def test_file(func):
    FILES.append(func)
    return func


@test_file
def empty():
    hdul = fits.HDUList([fits.PrimaryHDU()])
    hdul[0].header['COMMENT'] = 'This is a'
    hdul[0].header['COMMENT'] = 'multiline comment'
    return hdul

@test_file
def int16_image():
    primary = fits.PrimaryHDU(data=np.arange(12).reshape(4, 3).astype(np.int16))
    return fits.HDUList([primary])


@test_file
def uint16_image():
    primary = fits.PrimaryHDU(data=np.arange(12).reshape(4, 3).astype(np.uint16))
    return fits.HDUList([primary])


@test_file
def float_image():
    primary = fits.PrimaryHDU(data=np.arange(12).reshape(4, 3).astype(np.float32))
    return fits.HDUList([primary])

@test_file
def image_extension():
    primary = fits.PrimaryHDU()
    image = fits.ImageHDU(data=np.arange(12).reshape(4, 3).astype(np.int32))
    return fits.HDUList([primary, image])


def create_all():
    OUTPATH.mkdir(parents=True, exist_ok=True)

    with (OUTPATH / 'invalid.fits').open('wb') as f:
        f.write(b'not a fits file')

    for f in FILES:
        f().writeto(OUTPATH / (f.__name__ + '.fits'), overwrite=True)


if __name__ == '__main__':
    create_all()
