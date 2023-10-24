# huffman-compression

A simple cli tool to compress/decompress text (for now) files using [huffman coding algorithm](https://en.wikipedia.org/wiki/Huffman_coding).

## Getting started
```sh
# Clone the repository (SSH)
git clone --recurse-submodules git@github.com:srijanmukherjee/huffman-compression.git

# Clone the repository (HTTPS)
git clone --recurse-submodules https://github.com/srijanmukherjee/huffman-compression.git

# initialize build directory
make init

# to build the project
make

# to compress a text file
./build/huffman filepath

# to decompress
./build/huffman -d filepath
```

## Usage
```
Usage: huffman [--help] [--version] [--verbose] [--decompress] [--output VAR] file

Positional arguments:
  file              path to file 

Optional arguments:
  -h, --help        shows help message and exits 
  -v, --version     prints version information and exits 
  --verbose         enable verbose output 
  -d, --decompress  decompress file 
  -o, --output      output file path [default: ""]
```

## Result

- examples/sample0.txt
```
1009K sample0.txt.bin
1.8M  examples/sample0.txt
```

## Dependencies
- [p-ranav/argparse](https://github.com/p-ranav/argparse) for cli argument parsing

## License
This project is available under the [MIT](https://opensource.org/license/mit/) license