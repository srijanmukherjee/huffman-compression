#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

namespace huffman
{

class HuffmanTree
{
public:
    enum NodeType {
        Value,
        Intermediate
    };

    struct Node {
        std::shared_ptr<Node> left{nullptr};
        std::shared_ptr<Node> right{nullptr};
        uint32_t frequency;
        uint8_t value;
        NodeType type;

        Node(std::shared_ptr<Node> left, std::shared_ptr<Node> right)
            : left(left), right(right), frequency(left->frequency + right->frequency), type(Intermediate) {}
        Node(uint8_t value, uint32_t frequency): value(value), frequency(frequency), type(Value) {}
    };

    using NodePtr = std::shared_ptr<Node>;

    static NodePtr BuildTree(const std::map<uint8_t, uint32_t> &frequencies);
}; // class HuffmanTree

class Compressor
{
public:
    Compressor(std::ifstream &src);
    ~Compressor() = default;

    void WriteToFile(std::ofstream &dest);

private:
    void BuildTable();
    void BuildCharMap();

private:
    std::ifstream &m_srcFile;
    std::map<uint8_t, uint32_t> m_frequencyTable;
    std::map<uint8_t, std::string> m_charBitMap;
}; // class Compressor

class Decompressor
{
public:
    Decompressor(std::ifstream &compressedSrc);
    ~Decompressor() = default;

    void WriteToFile(std::ofstream &dest);

private:
    void BuildTable();

private:
    std::ifstream &m_compressedSrc;
    std::map<uint8_t, uint32_t> m_frequencyTable;
    std::shared_ptr<HuffmanTree::Node> m_treeRoot{nullptr};
    uint32_t m_offset{0};
    uint32_t m_totalDataSize{0};
}; // class Decompressor

}; // end namespace

#endif // __HUFFMAN_H__