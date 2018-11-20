#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <functional>
#include <map>
#include <bitset>


using namespace std;

enum HuffJamCommand
{
	notcommand,
	compress,
	decompress
};

struct HuffNode
{
	HuffNode* rightChild;
	HuffNode* leftChild;
	char val;
	bool original = false;
	string encoding;
	char originalChar;
};

HuffJamCommand ParseInput(int argsNum, char* argv[]) {
	if (argsNum < 3) {
		return notcommand;
	}
	else if (argv[0] == "compress")
	{
		return compress;
	}
	else if (argv[0] == "decompress") {
		return decompress;
	}
	else {
		return notcommand;
	}
}

class HuffComparator
{
public:
	bool operator()(const HuffNode& nodeOne, const HuffNode& nodeTwo) {
		return nodeOne.val > nodeTwo.val;
	}
};

inline void PrintInstructions() {
	cout << "Please, enter valid arguments\n";
	cout << "compress {input file} {output file}\t\tTo compress a file\n";
	cout << "decompress {input file} {output file}\t\tTo decompress a file\n";
}

void CompressFile(ifstream& InputFile, ofstream& OutputFile) {
	const long initialCount = 0;
	const long maxCharsNum = 256;
	
	//Counting Freq
	vector<long> Freqs(maxCharsNum, initialCount);
	string inputFileContent;
	getline(InputFile, inputFileContent, (char)InputFile.eof());
	char currentCharacter;
	for (char& currentCharacter : inputFileContent) {
		++Freqs[unsigned char(currentCharacter)];
	}

	//Doing Initial huffman encoding
	priority_queue<HuffNode, vector<HuffNode>, HuffComparator> huffEncoding;
	for (int i = 0; i <Freqs.size();++i) {
		if (Freqs[i] != initialCount) {
			huffEncoding.push(HuffNode{
				rig
				}
		}
	}

	//Optimizing the encoding
	while (huffEncoding.size() > 2) {
		auto nodeOne = huffEncoding.top();
		huffEncoding.pop();
		auto nodeTwo = huffEncoding.top();
		huffEncoding.pop();

		huffEncoding.push(HuffNode{ &nodeOne, &nodeTwo, (nodeOne.val + nodeTwo.val), false });
	}


	//preparing file header
	map<char, string> Mapping;
	while (huffEncoding.size() > 0) {
		 auto root = huffEncoding.top();
		 huffEncoding.pop();

		 if (!(root.original)) {
			 root.leftChild->encoding = root.encoding + "0";
			 root.rightChild->encoding = root.encoding + "1";

			 huffEncoding.push(*(root.leftChild));
			 huffEncoding.push(*(root.rightChild));
		 }
		 else {
			 Mapping[root.originalChar] = root.encoding;
		 }

	}

	//writing file header
	OutputFile << Mapping.size() << '\n';
	for (map<char, string>::iterator it = Mapping.begin(); it != Mapping.end(); ++it) {
		OutputFile << it->second << '\t' << it->first;
	}


	//write file content
	string OutputContentBinaryString = "";
	for (char& currentCharacter : inputFileContent)
	{
		OutputContentBinaryString = OutputContentBinaryString + Mapping[currentCharacter];
	}
	string OutputContent = "";
	long sz = OutputContentBinaryString.size();
	long charNum = sz / 8;
	for (long i; i < charNum; ++i) {
		bitset<8> num(OutputContentBinaryString.substr(i * 8, 8));
		OutputContent = OutputContent + char(num.to_ulong());
	}
	bitset<8> num(OutputContentBinaryString.substr(charNum * 8));
	OutputContent = OutputContent + char(num.to_ulong());

	OutputFile << OutputContent;
}

void DecompressFile(ifstream& InputFile, ofstream& OutputFile) {
	int encodingSize;

	InputFile >> encodingSize;
	map<string, char> Mapping;

	for (int i = 0; i < encodingSize; ++i) {
		string encoding;
		char seperator;
		char encodedChar;
		InputFile >> encoding >> seperator >> encodedChar;
	}

	string code = "";
	char NextCharacter;
	string OriginalContent = "";

	while (InputFile >> NextCharacter) {
		bitset<8> NextCharacterBits(NextCharacter);
		int i = 7;
		while (i > -1) {
			code += char(NextCharacterBits[i--] - '0') + code;

			map<string, char>::iterator decodedChar = Mapping.find(code);
			if (decodedChar != Mapping.end()) {
				OriginalContent += decodedChar->second;
				code = "";
			}
		}
	}
	OutputFile << OriginalContent;
}

void ExecuteCommand(const HuffJamCommand & cmd, const string & InputFileName, const string & OutputFileName) {
	ifstream InputFile(InputFileName);
	if (InputFile.is_open()) {
		ofstream OutputFile(OutputFileName, std::ofstream::out | std::ofstream::trunc);
		if (OutputFile.is_open()) {
			if (cmd == compress) {
				CompressFile(InputFile, OutputFile);
			}
			else if (cmd == decompress) {
				DecompressFile(InputFile, OutputFile);
			}
		}
		else {
			cout << "Error Opening Output File!\n";
		}		
	}
	else {
		cout << "Error Opening Input File!\n";
	}
}

int main(int argc, char *argv[]) {
	
	HuffJamCommand cmd = ParseInput(argc, argv);
	if (cmd == notcommand) {
		PrintInstructions();
	}
	else
	{
		ExecuteCommand(cmd, argv[1], argv[2]);
	}

	system("pause");
	return 0;
}