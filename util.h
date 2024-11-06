
#include <iostream>
#include <fstream>
#include <map>
#include <queue>          // std::priority_queue
#include <vector>         // std::vector
#include <functional>     // std::greater
#include <string>
#include "bitstream.h"
#include "hashmap.h"
#include "mymap.h"
#pragma once

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};


class prioritize {
 public:
    bool operator()(HuffmanNode* p1, HuffmanNode* p2) const {
        return p1->count > p2->count;
    }
};


//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode* node) {
    if (node == nullptr) {
        return;
    }
    freeTree(node->zero);
    freeTree(node->one);
    delete node;
}


//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmap &map) {
    if (!isFile) {
        for (char c : filename) {
            if (map.containsKey(c)) {
                int freq = map.get(c);
                freq++;
                map.put(c, freq);
            } else {
                map.put(c, 1);
            }
        }
    } else {
        ifstream inFS(filename);
        char c;
        while (inFS.get(c)) {
            if (map.containsKey(c)) {
                int freq = map.get(c);
                freq++;
                map.put(c, freq);
            } else {
                map.put(c, 1);
            }
        }
    }
    map.put(PSEUDO_EOF, 1);
}


//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmap &map) {
    vector<int> allKeys = map.keys();
    priority_queue <HuffmanNode*, vector<HuffmanNode*>, prioritize> pq;

    for (int i = 0; i < allKeys.size(); i++) {
        HuffmanNode *node = new HuffmanNode();
        node->count = map.get(allKeys[i]);
        node->character = allKeys[i];
        node->one = nullptr;
        node->zero = nullptr;
        pq.push(node);
    }

    while (pq.size() > 1) {

        HuffmanNode *newNode = new HuffmanNode();
        HuffmanNode *node1 = pq.top();
        pq.pop();
        HuffmanNode *node2 = pq.top();
        pq.pop();

        newNode->zero = node1;
        newNode->one = node2;
        newNode->count = node1->count + node2->count;
        newNode->character = NOT_A_CHAR;
        pq.push(newNode);
    }
    return pq.top();
}


//
// *This function recurisvly adds the characters to the map
//
void _buildEncodingMap(HuffmanNode* node, mymap <int, string>& encodingMap,
                       string value){
    if (node == nullptr) {
        return;
    } else if (node->character != NOT_A_CHAR) {
        encodingMap.put(node->character, value);
    }

    _buildEncodingMap(node->zero, encodingMap, value + "0");
    _buildEncodingMap(node->one, encodingMap, value + "1");
}


//
// *This function builds the encoding map from an encoding tree.
//
mymap <int, string> buildEncodingMap(HuffmanNode* tree) {

    mymap <int, string> encodingMap;
    if (tree == nullptr) {
        return encodingMap;
    }
    _buildEncodingMap(tree, encodingMap, "");

    return encodingMap;
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, mymap <int, string> &encodingMap,
              ofbitstream& output, int &size, bool makeFile) {
    char c;
    string binaryString = "";
    while (input.get(c)) {
        if (encodingMap.contains(c)) {
            binaryString += encodingMap.get(c);
        }
    }
    
    binaryString += encodingMap.get(PSEUDO_EOF);

    size =  binaryString.size();
    if (makeFile) {
        for (char c : binaryString) {
            if (c == '0') {
                output.writeBit(0);
            } else {
                output.writeBit(1);
            }
        }
    }
    return binaryString;
}


//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    HuffmanNode* curr = encodingTree;
    HuffmanNode* root = encodingTree;
    string outputFile;
    while (!input.eof()) {
        int bit = input.readBit();
        if (bit == 0) {
            curr = curr->zero;
        } else {
            curr = curr->one;
        }

        if (curr->character != PSEUDO_EOF && curr->character != NOT_A_CHAR) {
            outputFile+=curr->character;
            output.put(curr->character);
            curr = root;
        }
        if (curr->character == PSEUDO_EOF) {
            break;
        }
    }
    return outputFile;
}


//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
    ifstream input(filename);
    ofbitstream output(filename + ".huf");
    hashmap map;
    int size = 0;
    mymap <int, string> encodingMap;
    bool isFile = false;

    if (input.is_open()) {
        isFile = true;
    }

    buildFrequencyMap(filename, isFile, map);
    output << map;
    HuffmanNode* encodingTree = buildEncodingTree(map);
    encodingMap = buildEncodingMap(encodingTree);
    freeTree(encodingTree);


    return encode(input, encodingMap, output, size, isFile);
}


//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {

    string content = "";
    ifbitstream input(filename);
    hashmap map;
    mymap <int, string> encodingMap;
    bool isFile = false;

    if (input.is_open()) {
        isFile = true;
    }
    size_t pos = filename.find(".txt.huf");

    if ((int)pos >= 0) {
        filename = filename.substr(0, pos);
    }
    filename += "_unc.txt";

    ofstream output(filename);
    input >> map;
    buildFrequencyMap(filename, isFile, map);
    HuffmanNode* encodingTree = buildEncodingTree(map);
    content = decode(input, encodingTree, output);
    freeTree(encodingTree);

    return content;
}
