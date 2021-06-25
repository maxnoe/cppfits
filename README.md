# fitscpp - A FITS file reader in modern C++

Beware: this is a toy project for me learning this "new" language.

To properly learn a language, you need to project with sufficient complexity
in a field where you are comfortable, for me this is binary IO, especially with
the FITS format.


This project uses catch2 for unit tests and will probably make
use of boost and/or Abseil to solve specific tasks.

C++17 is required, mainly for `std::variant`, which is used for runtime
polymorphism, e.g. the value of a FITS header entry is a
```
using value_t = std::variant<std::string, bool, int64_t, double>;
```
and I plan to use this also for other polymorphic entities, like the
different types of FITS extension HDUs (Image, Binary Table, ASCII Table, ...).
