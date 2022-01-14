#include "HC_Interface.h"

bool HC_Interface::compressFileTo(const string& filePath,const string& destination) {

	Frequency_Map freqMap(filePath); //create freq map
	if (freqMap.getPairs() == nullptr) {
		return false;
	}

	Huffman_Tree mytree(freqMap); //create tree from freq map

	freqMap.writeFreqMapToFile(destination); //writes the freq map to the file header so it can be decoded later

	if (mytree.compressFileTo(freqMap,destination)) {

		cout << endl << "Original file size was " << freqMap.getTotalNumberOfChars() << " bytes." << endl;
		cout << "Compressed file size is " << HC_Interface::getCompressedFileSize(freqMap, mytree) << " bytes." << endl;
		std::cout << std::fixed;
		std::cout << std::setprecision(2);
		cout << "The compressed file is " << ((long double)HC_Interface::getCompressedFileSize(freqMap, mytree) /
			(long double)freqMap.getTotalNumberOfChars()) * 100 << "% of the original size." << endl<<endl;
	}
	else {
		std::cout << "Compression failed." << endl;
		return false;
	}
}

bool HC_Interface::decompressFileTo(const string& filePath,const string& destination) {
	ifstream fileInput(filePath, ios::binary);
	if (fileInput) {
		pair<unsigned char, frequency> pairs[256];
		for (short i = 0; i < 256; i++) {
			pairs[i].key = i;
			fileInput.read((char*)&pairs[i].freq,sizeof(frequency));
		}

		std::streampos pos = fileInput.tellg();
		fileInput.close();

		Frequency_Map mymap(pairs);

		Huffman_Tree mytree(mymap);

		if (mytree.decompressFileTo(filePath, pos, destination)) {
			cout << "Decompression to " << destination << " succeeded" << endl;
			return true;
		}
		else {
			std::cout << "Decompression failed" << endl;
			return false;
		}
	}
	else {
		std::cout << "Error! Failed reading from " << filePath << endl;
		return false;
	}
}

fileSize HC_Interface::getCompressedFileSize(const Frequency_Map& map, const Huffman_Tree& tree) {
	return 256*sizeof(frequency) + sizeof(fileSize) + tree.getCompressedStrSizeInBytes(map);
	//file structure is all 256 frequencies written at the beginning then size of actual coded data in bits
	//and finally the coded data at the end
}