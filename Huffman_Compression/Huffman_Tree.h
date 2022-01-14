#pragma once
#include "Frequency_Map.h"
#include <queue>
using std::queue;
#include <vector>
using std::vector;

struct CodedValues {
private:
	unsigned int code; //the binary code
	unsigned int size; //the length of the binary code
public:
	CodedValues():code(0),size(0) {}
	CodedValues(unsigned int code, unsigned int size):code(code),size(size) {}

	unsigned int getCode() const {
		return code;
	}
	unsigned int getSize() const {
		return size;
	}

	void setCode(unsigned int code) {
		this->code = code;
	}
	void setSize(unsigned int size) {
		this->size = size;
	}
};

struct Node {
	frequency freq;
	keyType data;
	Node* left;
	Node* right;

	Node() :freq(0), data(0), left(nullptr),right(nullptr) {}
	Node(const keyType& data,const frequency& freq) :freq(freq), data(data), left(nullptr), right(nullptr) {}

	Node(Node* left,Node* right) :data(0), left(left), right(right) {  //creates a new node combining the freq of the two
		this->freq = (*left).freq + (*right).freq;                     //argument nodes, having the two nodes as children
	}
};

struct CompareFreq {
	bool operator()(const Node* p1,const Node* p2) const {
		return (p1->freq) > (p2->freq);
	}
};

class Huffman_Tree {
private:
	Node* root;
	CodedValues* codedVal;

	string filePath;

	void combineSmallestNodes(std::priority_queue<Node*, vector<Node*>, CompareFreq>&);
	// creates new node combining the 2 smallest nodes,
	//removes the smallest nodes and inserts the combined one into list

	void deleteTree(const Node* root); // helps destructor

	void getCodes(); //creates an array of binary representations for the characters and stores it in codedVal
	void getCodesHelper(const Node*, CodedValues*, unsigned int, unsigned int) const;

	fileSize getCompressedLengthInBits(const Frequency_Map&,const CodedValues*) const;
	//calculates the bit size of the compressed file

	void adjustChunkSize(unsigned int&, const fileSize&) const;

public:
	Huffman_Tree(const Frequency_Map&);
	~Huffman_Tree();
	Huffman_Tree(const Huffman_Tree&) = delete;
	Huffman_Tree& operator=(const Huffman_Tree&) = delete;

	bool compressFileTo(const Frequency_Map&,const string& destination);
	//encodes the file
	
	bool decompressFileTo(const string& fileToReadFrom,const std::streampos&, const string& destination);
	//decodes the file

	fileSize getCompressedStrSizeInBytes(const Frequency_Map&) const; 
	//takes frequency map and  map with codes as arguments

	const CodedValues* getCodedValues() const;
	const Node* getRoot() const;
};