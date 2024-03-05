#include <iostream>
#include <fstream>
#include <queue>
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

// Comparator for priority queue
struct compare {
    bool operator()(Node* l, Node* r) {
        return l->frequency > r->frequency;
    }
};

// Function to generate Huffman Tree
Node* buildHuffmanTree(unordered_map<char, int>& freqMap) {
    priority_queue<Node*, vector<Node*>, compare> pq;
    
    // Create leaf nodes for each character and push them into the priority queue
    for (auto& pair : freqMap) {
        pq.push(new Node(pair.first, pair.second));
    }
    
    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        
        Node* newNode = new Node(left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;
        
        pq.push(newNode);
    }
    
    return pq.top();
}

// Function to generate Huffman Codes
void generateCodes(Node* root, string code, unordered_map<char, string>& codes) {
    if (!root) return;
    
    if (root->data != '\0') {
        codes[root->data] = code;
    }
    
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

// Function to write Huffman Codes to output file
void writeCodesToFile(string inputFile, unordered_map<char, string>& codes) {
    string outputFile = inputFile + ".huff";
    ofstream fout(outputFile, ios::out | ios::binary);
    if (!fout.is_open()) {
        cerr << "Error opening file for writing.";
        return;
    }
    
    // Writing the number of distinct characters
    int numChars = codes.size();
    fout.write(reinterpret_cast<const char*>(&numChars), sizeof(numChars));
    
    // Writing the characters and their Huffman Codes
    for (auto& pair : codes) {
        fout.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));
        
        string code = pair.second;
        int codeLength = code.length();
        fout.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength));
        
        fout.write(code.c_str(), codeLength);
    }
    
    fout.close();
}

// Function to compress the input file using Huffman Codes
void compressFile(string inputFile) {
    unordered_map<char, int> freqMap;
    
    // Counting the frequency of each character in the input file
    ifstream fin(inputFile);
    if (!fin.is_open()) {
        cerr << "Error opening file for reading.";
        return;
    }
    
    char ch;
    while (fin.get(ch)) {
        freqMap[ch]++;
    }
    fin.close();
    
    // Building the Huffman Tree
    Node* root = buildHuffmanTree(freqMap);
    
    // Generating Huffman Codes
    unordered_map<char, string> codes;
    generateCodes(root, "", codes);
    
    // Writing Huffman Codes to output file
    writeCodesToFile(inputFile, codes);
}

int main() {
    string inputFile;
    cout << "Enter the name of the file to be compressed: ";
    cin >> inputFile;
    
    compressFile(inputFile);
    
    cout << "File compressed successfully.\n";
    
    return 0;
}
