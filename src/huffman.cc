#include "huffman.h"

#include <exception>
#include <iostream>
#include <string>
#include <queue>

#define BUFFER_SIZE 1024

std::shared_ptr<huffman::HuffmanTree::Node> huffman::HuffmanTree::BuildTree(
    const std::map<uint8_t, uint32_t> &frequencies)
{
    std::priority_queue<NodePtr, std::vector<NodePtr>, std::greater<NodePtr>> pq;

    for (auto [key, frequency] : frequencies) {
        pq.emplace(std::make_shared<Node>(key, frequency));
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

    if (!m_srcFile.is_open()) {
        throw new std::runtime_error("source file is not open");
    }

    m_srcFile.seekg(0, std::ios_base::beg);

    // Write table
    outBuffer = m_frequencyTable.size();
    dest.write(reinterpret_cast<const char *>(&outBuffer), sizeof(outBuffer));

    for (auto [key, value] : m_frequencyTable) {
        outBuffer = key;
        dest.write(reinterpret_cast<const char *>(&outBuffer), sizeof(outBuffer));
        outBuffer = value;
        dest.write(reinterpret_cast<const char *>(&outBuffer), sizeof(outBuffer));
    }

    outBuffer = 0;

    // Write compressed data
    while (m_srcFile.getline(buf, sizeof(buf))) {
        for (int i = 0; i < sizeof(buf) && buf[i] != '\0'; i++) {
            auto bits = m_charBitMap[buf[i]];
            
            for (char c : bits) {
                uint8_t bit = c - '0';
                bitsUsed++;
                outBuffer = (outBuffer << 1) | bit;

                if (bitsUsed == sizeof(outBuffer) * 8) {
                    dest.write(reinterpret_cast<const char *>(&outBuffer), sizeof(outBuffer));
                    outBuffer = 0;
                    bitsUsed = 0;
                }
            }
        }

        // there's a newline
        if (m_srcFile.peek() != EOF) {
            auto bits = m_charBitMap['\n'];
            
            for (char c : bits) {
                uint8_t bit = c - '0';
                bitsUsed++;
                outBuffer = (outBuffer << 1) | bit;

                if (bitsUsed == sizeof(outBuffer) * 8) {
                    dest.write(reinterpret_cast<const char *>(&outBuffer), sizeof(outBuffer));
                    outBuffer = 0;
                    bitsUsed = 0;
                }
            }
        }
    }

    if (bitsUsed > 0) {
        dest.write(reinterpret_cast<const char *>(&outBuffer), sizeof(outBuffer));
    }

    dest.flush();
}