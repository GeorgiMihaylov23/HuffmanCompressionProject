#pragma once
#include <string>
#include <fstream>
#include <iostream>
using std::ifstream;
using std::ofstream;
using std::string;
using std::ios;
using std::cout;
using std::endl;

#define keyType unsigned char
#define frequency unsigned int

#define fileSize unsigned long long int

const std::size_t Byte = 1;
const std::size_t KB = 1024 * Byte;
const std::size_t MB = 1024 * KB;

#define MAX_BUFFER_SIZE 16*KB

template<class A,class B>
struct pair {
	A key;
	B freq;

	pair():key(),freq() {};
	pair(const A& key,const B& freq):key(key),freq(freq) {}

	pair<A,B>& operator= (const pair<A,B>& other) {
		this->key = other.key;
		this->freq = other.freq;
		return *this;
	}
};

class Frequency_Map {
private:
	pair<keyType,frequency> pairs[256];

	string filePath;

	frequency& operator[] (const keyType&);

	void adjustChunkSize(unsigned int&/*chunk*/,const fileSize&) const;
	//takes current chunk of data to read and bytes of the file left and adjusts the chunk size accordingly
	//to not be bigger than the data left

public:
	Frequency_Map() = delete;
	Frequency_Map(const string& filename); //reads the file and creates map based on contents
	Frequency_Map(const Frequency_Map&);
	Frequency_Map& operator=(const Frequency_Map&);
	~Frequency_Map();

	Frequency_Map(pair<unsigned char, frequency>*); // creates a freq map from an arr of pre exisitng pairs

	void print() const;

	void writeFreqMapToFile(const string& destination);

	const pair<unsigned char,frequency>* const getPairs() const;
	const string& getFilePath() const;
	fileSize getTotalNumberOfChars() const;
};