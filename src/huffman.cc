#include "huffman.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>
#include <queue>

#define BUFFER_SIZE 4096

class NodeCompare 
{
public:
    bool operator() (huffman::HuffmanTree::NodePtr a, huffman::HuffmanTree::NodePtr b) {
        return a->frequency > b->frequency;
    }
};

std::shared_ptr<huffman::HuffmanTree::Node> huffman::HuffmanTree::BuildTree(
    const std::map<uint8_t, uint32_t> &frequencies)
{
    std::priority_queue<NodePtr, std::vector<NodePtr>, NodeCompare> pq;

    for (auto [key, frequency] : frequencies) {
        pq.push(std::make_shared<Node>(key, frequency));
    }

    while (pq.size() > 1) {
        auto a = pq.top();
        pq.pop();
        auto b = pq.top();
        pq.pop();

        pq.emplace(std::make_shared<Node>(a, b));
    }

    return pq.top();
}

huffman::Compressor::Compressor(std::ifstream &src): m_srcFile(src)
{
    BuildTable();
    BuildCharMap();
}

void huffman::Compressor::BuildTable()
{  
    char buf[BUFFER_SIZE];
    while (m_srcFile.getline(buf, sizeof(buf))) {
        for (int i = 0; i < sizeof(buf) && buf[i] != '\0'; i++) {
            m_frequencyTable[buf[i]]++;
        }

        if (m_srcFile.peek() != EOF) {
            m_frequencyTable['\n']++;
        }
    }
    m_srcFile.clear();
}

void generateCharMap(std::map<uint8_t, std::string> &charMap, huffman::HuffmanTree::NodePtr node, std::string bits = "") {
    if (node == nullptr) {
        return;
    }

    if (node->type == huffman::HuffmanTree::NodeType::Value && node->left == nullptr && node->right == nullptr) {
        charMap[node->value] = bits;
        return;
    }

    generateCharMap(charMap, node->left, bits + "0");
    generateCharMap(charMap, node->right, bits + "1");
}

void huffman::Compressor::BuildCharMap() 
{
    auto huffmanTree = HuffmanTree::BuildTree(m_frequencyTable);
    generateCharMap(m_charBitMap, huffmanTree);
}

void huffman::Compressor::WriteToFile(std::ofstream &dest)
{
    char buf[BUFFER_SIZE];
    uint32_t outBuffer = 0;
    size_t bitsUsed = 0;
    uint32_t totalDataSize = 0;

    if (!m_srcFile.is_open()) {
        throw new std::runtime_error("source file is not open");
    }

    m_srcFile.seekg(0, std::ios_base::beg);

    // Write table
    outBuffer = m_frequencyTable.size();
    dest.write(reinterpret_cast<const char *>(&outBuffer), sizeof(outBuffer));

    for (auto [key, value] : m_frequencyTable) {
        dest.write(reinterpret_cast<const char *>(&key), sizeof(key));
        dest.write(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    outBuffer = 0;

    // Write compressed data
    while (m_srcFile.getline(buf, sizeof(buf))) {
        for (int i = 0; i < sizeof(buf) && buf[i] != '\0'; i++) {
            auto bits = m_charBitMap[buf[i]];
            
            for (char c : bits) {
                uint32_t bit = c == '1' ? 1 : 0;
                outBuffer = (outBuffer << 1) | bit;
                bitsUsed++;

                if (bitsUsed == sizeof(outBuffer) * 8) {
                    dest.write(reinterpret_cast<const char *>(&outBuffer), sizeof(outBuffer));
                    totalDataSize += bitsUsed;
                    outBuffer = 0;
                    bitsUsed = 0;
                }
            }
        }

        // there's a newline
        if (m_srcFile.peek() != EOF) {
            auto bits = m_charBitMap['\n'];
            
            for (char c : bits) {
                uint32_t bit = c == '1' ? 1 : 0;
                bitsUsed++;
                outBuffer = (outBuffer << 1) | bit;

                if (bitsUsed == sizeof(outBuffer) * 8) {
                    dest.write(reinterpret_cast<const char *>(&outBuffer), sizeof(outBuffer));
                    totalDataSize += bitsUsed;
                    outBuffer = 0;
                    bitsUsed = 0;
                }
            }
        }
    }

    if (bitsUsed > 0) {
        totalDataSize += bitsUsed;
        dest.write(reinterpret_cast<const char *>(&outBuffer), sizeof(outBuffer));
    }

    // write total compressed data size
    dest.write(reinterpret_cast<const char *>(&totalDataSize), sizeof(totalDataSize));
    dest.flush();
}

/****************************************************************/
/*                         Decompressor                         */
/****************************************************************/

huffman::Decompressor::Decompressor(std::ifstream &compressedSrc)
    : m_compressedSrc(compressedSrc)
{
    BuildTable();
    m_treeRoot = HuffmanTree::BuildTree(m_frequencyTable);
}

void huffman::Decompressor::BuildTable()
{
    uint32_t n = 0;
    uint8_t c = 0;
    uint32_t frequency = 0;

    if (!m_compressedSrc.is_open()) {
        throw new std::runtime_error("input file is not open");
    }

    m_compressedSrc.read(reinterpret_cast<char *>(&n), sizeof(n));

    for (int i = 0; i < n; i++) {
        m_compressedSrc.read(reinterpret_cast<char *>(&c), sizeof(c));
        m_compressedSrc.read(reinterpret_cast<char *>(&frequency), sizeof(frequency));
        m_frequencyTable[c] = frequency;
    }

    m_offset = sizeof(n) + n * (sizeof(c) + sizeof(frequency));

    m_compressedSrc.seekg(-sizeof(m_totalDataSize), std::ios::end);
    m_compressedSrc.read(reinterpret_cast<char *>(&m_totalDataSize), sizeof(m_totalDataSize));
}

void huffman::Decompressor::WriteToFile(std::ofstream &dest) 
{
    if (!m_compressedSrc.is_open()) {
        throw new std::runtime_error("input file is not open");
    }

    if (!dest.is_open()) {
        throw new std::runtime_error("output file is not open");
    }

    m_compressedSrc.clear();
    m_compressedSrc.seekg(m_offset, std::ios::beg);

    auto currentState = m_treeRoot;
    uint32_t inBuf;
    size_t bitsLeft = m_totalDataSize;

    while (bitsLeft > 0 && !m_compressedSrc.eof()) {
        m_compressedSrc.read(reinterpret_cast<char *>(&inBuf), sizeof(inBuf));
        size_t offset = std::min(bitsLeft, sizeof(inBuf) * 8);

        for (ssize_t i = offset - 1; i >= 0; i--) {
            auto bit = (inBuf >> i) & 1;

            if (bit == 0) {
                currentState = currentState->left;
            } else {
                currentState = currentState->right;
            }

            if (currentState->type == HuffmanTree::NodeType::Value) {
                dest << (char) currentState->value;
                currentState = m_treeRoot;
            }
        }

        bitsLeft -= offset;
    }
}