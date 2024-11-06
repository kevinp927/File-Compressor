//
//  main.cpp
//  File Compression II
//
//  Created by Keyur Patel on 4/2/22.
//

#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <queue>
#include <vector>
#include <functional>
#include <ctype.h>
#include <math.h>
#include "bitstream.h"
#include "util.h"
#include "hashmap.h"

using namespace std;

string menu();
bool is123456(string choice);
void do123456(string choice, string &filename, bool &isFile,
             hashmap &frequencyMap,
             HuffmanNode* &encodingTree,
             mymap <int, string> &encodingMap);


int main(int argc, const char * argv[]) {
    hashmap frequencyMap;
    HuffmanNode* encodingTree = nullptr;
    mymap <int, string> encodingMap;
    string filename;
    bool isFile = true;
    
    
    string choice = "weeeee";
    while (choice != "Q") {
        choice = menu();
        if (is123456(choice)){
            do123456(choice, filename, isFile, frequencyMap,
                    encodingTree, encodingMap);
        } else if (choice == "C") {
            cout << "Enter filename: ";
            cin >> filename;
            compress(filename);
        } else if (choice == "D") {
            cout << "Enter filename: ";
            cin >> filename;
            decompress(filename);
        } else if (choice == "B") {
            cout << "Enter filename: ";
            cin >> filename;
            //printBinaryFile(filename);
        } else if (choice == "T") {
            cout << "Enter filename: ";
            cin >> filename;
            //printTextFile(filename);
        }
    }
    return 0;
}
string menu() {
    cout << "Welcome to the file compression app!" << endl;
    cout << "1.  Build character frequency map" << endl;
    cout << "2.  Build encoding tree" << endl;
    cout << "3.  Build encoding map" << endl;
    cout << "4.  Encode data" << endl;
    cout << "5.  Decode data" << endl;
    cout << "6.  Free tree memory" << endl;
    cout << endl;
    cout << "C.  Compress file" << endl;
    cout << "D.  Decompress file" << endl;
    cout << endl;
    cout << "B.  Binary file viewer" << endl;
    cout << "T.  Text file viewer" << endl;
    cout << "Q.  Quit" << endl;
    cout << endl;
    
    cout << "Enter choice: ";
    string choice;
    cin >> choice;
    return choice;
}

bool is123456(string choice) {
    if (choice == "1" || choice == "2" ||choice == "3" ||
        choice == "4" ||choice == "5" || choice == "6") {
        return true;
    } else {
        return false;
    }
}

//
// do123456
// This function runs code for choice equal to 1, 2, 3, 4, 5, and 6.
// Correct, this is not properly decomposed.
//
void do123456(string choice, string &filename, bool &isFile,
             hashmap &frequencyMap,
             HuffmanNode* &encodingTree,
             mymap <int, string> &encodingMap) {
    // gets file/string and filename.
    if (choice == "1") {
        cout << "[F]ilename or [S]tring? ";
        string fORs;
        cin >> fORs;
        isFile = (fORs == "F" ? true : false);
        if (isFile) {
            cout << "Enter file name: ";
        } else {
            cout << "Enter string: ";
        }
        cin >> filename;
    }
    // Build Frequency Map
    if (choice == "1") {
        buildFrequencyMap(filename, isFile, frequencyMap);
        cout << endl;
        cout << "Building frequency map..." << endl;
        cout << endl;
    // Build Encoding Tree
    } else if (choice == "2") {
        encodingTree = buildEncodingTree(frequencyMap);
        cout << endl;
        cout << "Building encoding tree..." << endl;
        cout << endl;
    // Build Encoding Map
    } else if (choice == "3") {
        encodingMap = buildEncodingMap(encodingTree);
        cout << endl;
        cout << "Building encoding map..." << endl;
        cout << endl;
    // Encode text
    } else if (choice == "4") {
        // this step is only valid for files
        if (!isFile) {
            cout << endl;
            cout << "********************************" << endl;
            cout << "Must provide file to run encode." << endl;
            cout << "Enter Q to start over and try again." << endl;
            cout << "********************************" << endl;
            cout << endl;
            return;
        }
        cout << endl;
        cout << "Encoding..." << endl;
        
        string fn = (isFile) ? filename : ("file_" + filename + ".txt");
        
        ofbitstream output(filename + ".huf");
        ifstream input(filename);
        
        stringstream ss;
        // note: << is overloaded for the hashmap class.  super nice!
        ss << frequencyMap;
        output << frequencyMap;  // add the frequency map to the file
        int size = 0;
        string codeStr = encode(input, encodingMap, output, size, true);
        // count bytes in frequency map header
        size = ss.str().length() + ceil((double)size / 8);
        cout << "Compressed file size: " << size << endl;
        cout << codeStr << endl;
        cout << endl;
        output.close();  // must close file so autograder can open for testing
    // Decode text
    } else if (choice == "5") {
        // this step is only valid for files
        if (!isFile) {
            cout << endl;
            cout << "********************************" << endl;
            cout << "Must provide file to run encode." << endl;
            cout << "Enter Q to start over and try again." << endl;
            cout << "********************************" << endl;
            cout << endl;
            return;
        }
        cout << endl;
        cout << "Decoding..." << endl;
        size_t pos = filename.find(".huf");
        if ((int)pos >= 0) {
            filename = filename.substr(0, pos);
        }
        pos = filename.find(".");
        string ext = filename.substr(pos, filename.length() - pos);
        filename = filename.substr(0, pos);
        ifbitstream input(filename + ext + ".huf");
        ofstream output(filename + "_unc" + ext);
        
        hashmap dump;
        input >> dump;  // get rid of frequency map at top of file
        
        string decodeStr  = decode(input, encodingTree, output);
        cout << decodeStr << endl;
        cout << endl;
        output.close(); // must close file so autograder can open for testing
    // Free the Encoding Tree
    } else if (choice == "6") {
        cout << "Freeing encoding tree..." << endl;
        freeTree(encodingTree);
    }
}
