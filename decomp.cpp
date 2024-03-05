#include <iostream>
#include <fstream>
#include <unordered_map>
using namespace std;

// Node structure for Huffman Tree
struct Node {
    char data;
    int frequency;
    Node* left;
    Node* right;
    
    Node(char data, int frequency) : data(data), frequency(frequency), left(nullptr), right(nullptr) {}
    Node(int frequency) : data('\0'), frequency(frequency), left(nullptr), right(nullptr) {}
};

// Function to read Huffman Codes from input file
unordered_map<char, string> readCodesFromFile(string inputFile) {
    unordered_map<char, string> codes;
    ifstream fin(inputFile, ios::binary);
    if (!fin.is_open()) {
        cerr << "Error opening file for reading.";
        return codes;
    }
    
    // Reading the number of distinct characters
    int numChars;
    fin.read(reinterpret_cast<char*>(&numChars), sizeof(numChars));
    
    // Reading the characters and their Huffman Codes
    for (int i = 0; i < numChars; ++i) {
        char ch;
        fin.read(reinterpret_cast<char*>(&ch), sizeof(ch));
        
        int codeLength;
        fin.read(reinterpret_cast<char*>(&codeLength), sizeof(codeLength));
        
        char* buffer = new char[codeLength + 1];
        fin.read(buffer, codeLength);
        buffer[codeLength] = '\0';
        
        codes[ch] = buffer;
        delete[] buffer;
    }
    
    fin.close();
    return codes;
}

// Function to decompress the input file using Huffman Codes
void decompressFile(string inputFile, unordered_map<char, string>& codes) {
    string outputFile = inputFile.substr(0, inputFile.find_last_of('.')); // remove ".huff" extension
    ofstream fout(outputFile, ios::binary);
    if (!fout.is_open()) {
        cerr << "Error opening file for writing.";
        return;
    }
    
    ifstream fin(inputFile, ios::binary);
    if (!fin.is_open()) {
        cerr << "Error opening file for reading.";
        return;
    }
    
    fin.seekg(0, fin.end);
    int compressedFileSize = fin.tellg();
    fin.seekg(sizeof(int), fin.beg); // skip the number of distinct characters
    
    // Building the Huffman Tree
    Node* root = new Node(0);
    for (auto& pair : codes) {
        Node* curr = root;
        for (char bit : pair.second) {
            if (bit == '0') {
                if (!curr->left) {
                    curr->left = new Node(0);
                }
                curr = curr->left;
            } else {
                if (!curr->right) {
                    curr->right = new Node(0);
                }
                curr = curr->right;
            }
        }
        curr->data = pair.first;
    }
    
    // Decompressing the data
    int bitIndex = 0;
    char byte;
    Node* curr = root;
    while (fin.get(byte) && compressedFileSize > fin.tellg()) {
        for (int i = 0; i < 8; ++i) {
            if (byte & (1 << (7 - i))) {
                curr = curr->right;
            } else {
                curr = curr->left;
            }
            if (curr->data != '\0') {
                fout.put(curr->data);
                curr = root;
            }
        }
    }
    
    // Handle the last byte if necessary
    for (int i = 0; i < bitIndex; ++i) {
        if (byte & (1 << (7 - i))) {
            curr = curr->right;
        } else {
            curr = curr->left;
        }
        if (curr->data != '\0') {
            fout.put(curr->data);
            curr = root;
        }
    }
    
    fin.close();
    fout.close();
}

int main() {
    string inputFile;
    cout << "Enter the name of the compressed file to be decompressed: ";
    cin >> inputFile;
    
    unordered_map<char, string> codes = readCodesFromFile(inputFile);
    if (codes.empty()) {
        cerr << "Failed to read Huffman codes from file.\n";
        return 1;
    }
    
    decompressFile(inputFile, codes);
    
    cout << "File decompressed successfully.\n";
    
    return 0;
}
