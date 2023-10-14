#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include "huffman.h"

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    std::ifstream srcFile(argv[1]);

    if (!srcFile.is_open()) {
        fprintf(stderr, "Couldn't open file '%s': %s\n", argv[1], strerror(errno));
        return EXIT_FAILURE;
    }

    auto compressor = huffman::Compressor(srcFile);

    std::ofstream destFile("compressed.bin", std::ios::out | std::ios::binary);

    try {
        compressor.WriteToFile(destFile);
    } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
    }

    destFile.close();
    srcFile.close();

    return 0;
}
