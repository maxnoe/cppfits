#include <cstddef>
#include <string>

const std::string MARKER = "SIMPLE  =                    T";
const size_t BLOCK_SIZE = 2880;
const size_t ENTRY_SIZE = 80;
const size_t N_ENTRIES_BLOCK = BLOCK_SIZE / ENTRY_SIZE;
