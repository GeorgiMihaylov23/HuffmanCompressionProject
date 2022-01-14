#include "Frequency_Map.h"
#include "Huffman_Tree.h"
#include "HC_Interface.h"
#include <iostream>
using std::endl;
using std::cout;

int main(int argc,char** argv) {

	if (argc == 3) {
		string str1(argv[1]);
		//path of file to be compressed
		string str2(argv[2]);
		str2 += "compressed.dat";
		//path of file to be compressed to
		string str3(argv[2]);
		str3 += "decompressed";
		str3 += str1.substr(str1.find_last_of('.'), string::npos);
		//path of file to decompress to

		HC_Interface::compressFileTo(str1, str2);

		HC_Interface::decompressFileTo(str2, str3);
	}
	else {
		cout << "Invalid arguments." << endl;
	}
	
	return 0;
}