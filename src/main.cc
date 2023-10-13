#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <map>
#include <memory>
#include <ostream>
#include <queue>
#include <string>

// input: string
// 1. create table
// 2. create huffman tree
// 3. encode 

namespace huffman 
{
    struct Node {
        const std::shared_ptr<Node> left{nullptr};
        const std::shared_ptr<Node> right{nullptr};
        size_t frequency;

        Node(size_t frequency) : frequency(frequency) { }
        Node(std::shared_ptr<Node> left, std::shared_ptr<Node> right) 
            : frequency(left->frequency + right->frequency), left(left), right(right) { }

        bool operator<(const Node &rhs) const {
            return frequency < rhs.frequency;
        }
    };

    struct ValueNode : Node {
        uint8_t value;

        ValueNode(uint8_t value, size_t frequency) : value(value), Node(frequency) { }
    };

    // TODO: add support for unicodes, utf8
    using Table = std::map<uint8_t, size_t>;
    using CharBitMap = std::map<uint8_t, std::string>;


    /**
     * @brief creates a character frequency map
     * 
     * @param src 
     * @return std::unique_ptr<const Table> 
     */
    std::unique_ptr<const Table> build_table(std::string &src) {
        auto table = std::make_unique<Table>();

        for (uint8_t c : src) {
            (*table)[c]++;
        }

        return table;
    }

    std::shared_ptr<const Node> build_tree(std::unique_ptr<const Table> &table) {
        using NodePtr = std::shared_ptr<Node>;
        std::priority_queue<NodePtr, std::vector<NodePtr>, std::greater<NodePtr>> pq;

        for (auto it = table->begin(); it != table->end(); it++) {
            pq.emplace(std::make_shared<ValueNode>(it->first, it->second));
        }

        while (pq.size() > 1) {
            auto a = pq.top(); pq.pop();
            auto b = pq.top(); pq.pop();
            pq.emplace(std::make_shared<Node>(a, b));
        }

        return pq.top();
    }

    namespace {
        void _build_bit_map(std::unique_ptr<CharBitMap> &charBitMap, std::shared_ptr<const Node> node, std::string bits = "") {
            if (node == nullptr)
                return;

            // terminal node
            if (node->left == nullptr && node->right == nullptr) {
                ValueNode *valueNode = (ValueNode*) node.get();
                (*charBitMap)[valueNode->value] = bits;
            }

            _build_bit_map(charBitMap, node->left, bits + "0");
            _build_bit_map(charBitMap, node->right, bits + "1");
        }
    };

    std::unique_ptr<const CharBitMap> build_bit_map(std::shared_ptr<const Node> tree) {
        auto charBitMap = std::make_unique<CharBitMap>();

        _build_bit_map(charBitMap, tree);

        return charBitMap;
    }

}; // namespace huffman

int main(void) {
    std::string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

    for (int i = 0; i < 500; i++) {
        text = text + "lorem ipsum";
    }

    auto table = huffman::build_table(text);
    auto tree = huffman::build_tree(table);
    auto bitMap = huffman::build_bit_map(tree);

    size_t uncompressedSize = text.length() * 8; // bits
    size_t compressedSize = table->size() * (8 + 32); // table size (1B character, 4B frequency)

    for (uint8_t c : text) {
        compressedSize += bitMap->at(c).length();
    }

    double compressionRatio = (uncompressedSize - compressedSize) * 1.0 / uncompressedSize;

    std::cout << "compression ratio: " << compressionRatio << std::endl;

    // std::ofstream out("compressed.bin", std::ios::out | std::ios::binary);

    // for (uint8_t c : text) {
    //     out << bitMap->at(c);
    // }

    // out.close();

    return 0;
}