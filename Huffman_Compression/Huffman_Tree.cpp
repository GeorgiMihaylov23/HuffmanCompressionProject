#include "Huffman_Tree.h"

Huffman_Tree::Huffman_Tree(const Frequency_Map& map):filePath(map.getFilePath()) {

	std::priority_queue<Node*,vector<Node*>,CompareFreq> q;

	for (short i = 0; i < 256; i++) {
		if (map.getPairs()[i].freq > 0) {
			Node* newNode = new Node(map.getPairs()[i].key, map.getPairs()[i].freq);
			q.push(newNode);
		}
	}

	combineSmallestNodes(q);

	this->root = q.top();

	this->getCodes();
}

void Huffman_Tree::combineSmallestNodes(std::priority_queue<Node*, vector<Node*>, CompareFreq>& q) {

	while (q.size() != 1) {
		Node* node1 = q.top();
		q.pop();
		Node* node2 = q.top();
		q.pop();
		Node* newNode = new Node(node1, node2);
		q.push(newNode);
	}
}

Huffman_Tree::~Huffman_Tree() {
	delete[] codedVal;
	deleteTree(this->root);
}

void Huffman_Tree::deleteTree(const Node* root) {
	if (root) {
		deleteTree(root->left);
		deleteTree(root->right);
		delete root;
	}
}

const CodedValues* Huffman_Tree::getCodedValues() const
{
	return this->codedVal;
}

bool Huffman_Tree::compressFileTo(const Frequency_Map& freqMap,const string& destination) {
	ifstream fileInput(this->filePath, ios::binary);
	if (fileInput) {

		fileSize compressedStrLengthInBits = getCompressedLengthInBits(freqMap,this->codedVal);

		fileInput.seekg(0, ios::end);
		fileSize inputFileSize = fileInput.tellg();
		fileInput.seekg(0, ios::beg);

		unsigned int chunkSize = MAX_BUFFER_SIZE;
		unsigned char* fileChunk;
		
		ofstream output(destination, ios::app | ios::binary);
		if (output) {
			output.write(reinterpret_cast<char*>(&compressedStrLengthInBits), sizeof(compressedStrLengthInBits));

			unsigned int buffer = 0;
			int bufferSize = 0;

			while (inputFileSize>0) {

				adjustChunkSize(chunkSize, inputFileSize);
				try {
					fileChunk = new unsigned char[chunkSize];
				}
				catch (const std::bad_alloc & e) {
					std::cout << "Could not allocate memory.Error:" << e.what();
					std::abort();
				}
				fileInput.read((char*)fileChunk, chunkSize);

				for (unsigned int i = 0; i < chunkSize; i++) {
					if (bufferSize + codedVal[fileChunk[i]].getSize() == 32) {
						buffer = buffer << codedVal[fileChunk[i]].getSize() | codedVal[fileChunk[i]].getCode();

						output.write((char*)&buffer, 4);

						buffer = 0;
						bufferSize = 0;
					}
					else if ((bufferSize + codedVal[fileChunk[i]].getSize()) > 32) {
						unsigned int a = bufferSize + codedVal[fileChunk[i]].getSize() - 32;
						buffer = buffer << (32 - bufferSize) | (codedVal[fileChunk[i]].getCode() >> a);

						output.write((char*)&buffer, 4);

						buffer = 0;
						buffer = buffer | (codedVal[fileChunk[i]].getCode() <<(32-a) >> (32-a));
						bufferSize = a;
					}
					else {

						buffer = buffer << codedVal[fileChunk[i]].getSize() | codedVal[fileChunk[i]].getCode();
						bufferSize += codedVal[fileChunk[i]].getSize();
					}
				}
				inputFileSize -= chunkSize;
				delete[] fileChunk;
			}

			if (bufferSize > 0) {
				buffer = buffer << (32 - bufferSize);
				output.write((char*)&buffer, 4);
			}
			return true;
		}
		else {
			std::cout << "Error! Could not open " << destination << " for writing." << endl;
			return false;
		}
		output.close();
	}
	else {
		std::cout << "Could not open " << this->filePath << " for reading." << endl;
		return false;
	}
	fileInput.close();
}

bool Huffman_Tree::decompressFileTo(const string& filePath, const std::streampos& pos, const string& destination) {

	ifstream fileInput(filePath, ios::binary);
	if (fileInput) {
		fileInput.seekg(pos);

		fileSize sizeBits;
		fileInput.read(reinterpret_cast<char*>(&sizeBits), sizeof(sizeBits));

		unsigned int chunkSize = MAX_BUFFER_SIZE;
		unsigned int* fileChunk;

		fileSize sizeBytes;

		if (sizeBits % 8 > 0) {
			sizeBytes = sizeBits / 8 + 1;
		}
		else {
			sizeBytes = sizeBits / 8;
		}
		if (sizeBytes % 4 > 0) {
			sizeBytes += 4 - (sizeBytes % 4);
		}

		ofstream output(destination, ios::out | ios::binary);
		if (output) {

			Node* node = this->root;
			bool bit;
			unsigned char buffer = 0;

			while (sizeBytes > 0) {
				
				adjustChunkSize(chunkSize, sizeBytes);
				try {
					fileChunk = new unsigned int[chunkSize/4];
				}
				catch (const std::bad_alloc & e) {
					std::cout << "Could not allocate memory.Error:" << e.what();
					std::abort();
				}

				fileInput.read((char*)fileChunk, chunkSize);

				for (unsigned int i = 0; i<chunkSize/4; i++) {
					for (short j = 0; j < 32; j++) {
						if (sizeBits == 0) {
							break;
						}
						buffer = fileChunk[i] >> 31-j & 1;
						bit = (bool)buffer;
						if (bit) {
							node = node->right;
						}
						else {
							node = node->left;
						}
						if (node->left == nullptr && node->right == nullptr) {
							output.write((char*)&(node->data), 1);
							node = root;
						}
						sizeBits--;
					}
					sizeBytes -= 4;
				}
				delete[] fileChunk;
			}
		}
		else {
			std::cout << "Error! Could not open " << destination << " for writing." << endl;
			return false;
		}
		output.close();
	}
	else {
		std::cout << "Error! Failed reading from " << filePath << endl;
		return false;
	}
}

void Huffman_Tree::getCodes() {
	this->codedVal = new CodedValues[256];
	getCodesHelper(this->root, codedVal,0,0);
}

void Huffman_Tree::getCodesHelper(const Node* node,CodedValues* codedVal,unsigned int code,unsigned int size) const {
	if (node->left == nullptr && node->right == nullptr) {
		codedVal[node->data].setCode(code);
		codedVal[node->data].setSize(size);
		return;
	}

	getCodesHelper(node->left, codedVal,code<<1,size+1);
	getCodesHelper(node->right, codedVal,(code<<1)|1,size+1);
}

void Huffman_Tree::adjustChunkSize(unsigned int& chunkSize, const fileSize& sizeOfFile) const {
	while (chunkSize > sizeOfFile && sizeOfFile!=0) {
		chunkSize /= 2;
	}
}

fileSize Huffman_Tree::getCompressedStrSizeInBytes(const Frequency_Map& freqMap) const {

	fileSize size = getCompressedLengthInBits(freqMap, this->codedVal);

	if (size % 8 > 0) {
		return size / 8 + 1;
	}
	else {
		return size / 8;
	}
}

fileSize Huffman_Tree::getCompressedLengthInBits(const Frequency_Map& freqMap, const CodedValues* codedVal) const {

	fileSize compressedStrLength = 0; //in bits
	const pair<keyType, frequency>* pairs = freqMap.getPairs();

	for (short i = 0; i < 256; i++) {
		compressedStrLength += (fileSize)freqMap.getPairs()[i].freq * codedVal[i].getSize();
	}
	return compressedStrLength;
}

const Node* Huffman_Tree::getRoot() const {
	return root;
}