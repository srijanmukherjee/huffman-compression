#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>
#include <filesystem>

#include "huffman.h"
#include "../lib/argparse/include/argparse/argparse.hpp"

#define EXTENSION ".bin"

namespace fs = std::filesystem;

int main(int argc, const char *argv[]) {
    // TODO: pass version
    argparse::ArgumentParser program("huffman");

    program.add_argument("file")
        .help("path to file");
    
    program.add_argument("--verbose")
        .help("enable verbose output")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-d", "--decompress")
        .help("decompress file")
        .default_value(false)
        .implicit_value(true);
    
    program.add_argument("-o", "--output")
        .default_value(std::string("")) 
        .help("output file path");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    std::string srcFilePath = program.get<std::string>("file");
    std::string destFilePath = program.get<std::string>("--output");
    bool shouldDecompress = program.get<bool>("--decompress");

    if (!fs::exists(srcFilePath)) {
        fprintf(stderr, "ERROR: File doesn't exist\n");
        return EXIT_FAILURE;
    }

    if (fs::is_directory(srcFilePath)) {
        fprintf(stderr, "ERROR: Directories are not supported yet\n");
        return EXIT_FAILURE;
    }

    if (fs::is_symlink(srcFilePath)) {
        fprintf(stderr, "ERROR: Symlinks are not supported yet\n");
        return EXIT_FAILURE;
    }

    if (destFilePath == "") {
        auto path = fs::path(srcFilePath);

        if (shouldDecompress) {
            if (path.extension() == EXTENSION) {
                path = path.replace_extension("");
            }
            destFilePath = path.filename();
        } else {
            destFilePath = fs::path(srcFilePath).filename();
            destFilePath = destFilePath + EXTENSION;
        }
    }

    else {
        auto path = fs::path(destFilePath);

        if (fs::is_directory(path)) {
            destFilePath = fs::path(srcFilePath).filename();
            destFilePath = destFilePath + EXTENSION;
            path.append(destFilePath);
            destFilePath = path.string();
        }
    }

    std::ifstream srcFile(srcFilePath, std::ios::in | std::ios::binary);
    std::ofstream destFile(destFilePath, std::ios::out | std::ios::binary);

    if (!srcFile.is_open()) {
        fprintf(stderr, "Couldn't open file '%s': %s\n", srcFilePath.c_str(), strerror(errno));
        return EXIT_FAILURE;
    }

    if (!destFile.is_open()) {
        fprintf(stderr, "Couldn't open file '%s': %s\n", destFilePath.c_str(), strerror(errno));
        return EXIT_FAILURE;
    }

    if (shouldDecompress) {
        huffman::Decompressor decompressor(srcFile);
        try {
            decompressor.WriteToFile(destFile);
        } catch (std::exception e) {
            std::cerr << e.what() << std::endl;
        }
    } else {
        huffman::Compressor compressor(srcFile);
        try {
            compressor.WriteToFile(destFile);
        } catch (std::exception e) {
            std::cerr << e.what() << std::endl;
        }
    }

    srcFile.close();
    destFile.close();

    return 0;
}