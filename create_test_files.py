from astropy.io import fits
import numpy as np

def invalid():
    with open('./tests/data/invalid', 'wb') as f:
        f.write(b'not a fits file')


def empty():
    hdul = fits.HDUList([fits.PrimaryHDU()])
    hdul.writeto('tests/data/empty_primary_only.fits', overwrite=True)


def simple_image():
    primary = fits.PrimaryHDU(data=np.arange(12).reshape(4, 3).astype(np.uint16))
    hdul = fits.HDUList([primary])
    hdul.writeto('tests/data/simple_image.fits', overwrite=True)

def image_extension():
    primary = fits.PrimaryHDU()
    image = fits.ImageHDU(data=np.arange(12).reshape(4, 3).astype(np.uint16))
    hdul = fits.HDUList([primary, image])
    hdul.writeto('tests/data/image_extension.fits', overwrite=True)


def create_all():
    empty()
    simple_image()
    image_extension()


if __name__ == '__main__':
    create_all()
