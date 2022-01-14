#pragma once
#include "Huffman_Tree.h"
#include "Frequency_Map.h"
#include <iomanip>

struct HC_Interface {
	static bool compressFileTo(const string& filePath, const string& destination);
	static bool decompressFileTo(const string& filePath, const string& destination);

	~HC_Interface() = delete;
	void* operator new(size_t size) = delete;

private:
	static fileSize getCompressedFileSize(const Frequency_Map&, const Huffman_Tree&);
};