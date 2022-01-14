#include "Frequency_Map.h"

void Frequency_Map::adjustChunkSize(unsigned int& chunkSize,const fileSize& sizeOfFile)const {
	while (chunkSize > sizeOfFile && chunkSize >= Byte) {
		chunkSize /= 2;
	}
}

Frequency_Map::Frequency_Map(const string& filePath):filePath(filePath) {

	for(short i = 0; i < 256; i++) {
		pairs[i].key = (keyType)i;
		pairs[i].freq = 0;
	}

	ifstream fileInput(filePath, ios::binary );
	if (fileInput) {
		fileInput.seekg(0, ios::end);
		fileSize fileLength = fileInput.tellg(); //length of the file in bytes
		fileInput.seekg(0, ios::beg);

		unsigned int bufferSize = MAX_BUFFER_SIZE; 
		unsigned char* buffer;

		while (true) {
			if (fileLength == 0) {
				break;
			}
			adjustChunkSize(bufferSize, fileLength);

			try {
				buffer = new unsigned char[bufferSize];
			}
			catch (const std::bad_alloc& e) {
				std::cout << "Could not allocate memory.Error:" << e.what();
				std::abort();
			}

			fileInput.read((char*)buffer, bufferSize);

			for (unsigned int i = 0; i < bufferSize; i++) {
				(*this)[buffer[i]]++;
			}
			delete[] buffer;
			fileLength -= bufferSize;
		}
	}
	else {
		std::cout<<"Could not open "<<filePath<<" for reading.";
		std::abort();
	}
	fileInput.close();

	this->filePath = filePath;
}

Frequency_Map::Frequency_Map(const Frequency_Map& other) {
	for (short i = 0; i < 256; i++) {
		this->pairs[i] = other.pairs[i]; 
	}
}

Frequency_Map& Frequency_Map::operator=(const Frequency_Map& other) {
	if (this != &other) {
		for (short i = 0; i < 256; i++) {
			this->pairs[i] = other.pairs[i];
		}
	}
	return *this;
}

Frequency_Map::~Frequency_Map() {}

Frequency_Map::Frequency_Map(pair<unsigned char, frequency>* pairs) :filePath("") {
	for (short i = 0; i < 256; i++) {
		this->pairs[i].key = pairs[i].key;
		this->pairs[i].freq = pairs[i].freq;
	}
}

void Frequency_Map::print() const {  //prints the freq map to std output
	for (short i = 0; i < 256; i++) {
		cout <<i<<"-> "<<pairs[i].key << " : " << pairs[i].freq << endl;
	}
}

void Frequency_Map::writeFreqMapToFile(const string& destination) {
	/*
	I know my header is inefficient but i decided to do it 
	this way so the process is simpler and faster
	*/

	ofstream output(destination,ios::out | ios::binary);
	if (output) {
		for (short i = 0; i < 256; i++) {
			output.write((char*)&(pairs[i].freq), sizeof(frequency)); //writes pairs
		}
	}
	else {
		std::cout << "Error! Could not write to " << destination << endl;
	}
	output.close();
}

const pair<unsigned char,frequency>* const Frequency_Map::getPairs() const {
	return this->pairs;
}

const string& Frequency_Map::getFilePath() const {
	return this->filePath;
}

fileSize Frequency_Map::getTotalNumberOfChars() const {
	fileSize size=0;
	for (short i = 0; i < 256; i++) {
		size += pairs[i].freq;
	}
	return size;
}

frequency& Frequency_Map::operator[](const keyType& key) {
	return pairs[key].freq;
}