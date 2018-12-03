#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <functional>
#include <map>
#include <bitset>
#include <algorithm>


using namespace std;

enum HuffJamCommand
{
	notcommand,
	compress,
	decompress
};

struct HuffNode
{
	HuffNode* parent;
	HuffNode* rightChild;
	HuffNode* leftChild;
	long val;
	bool original = false;
	bool parentExist = false;
	string encoding = "0";
	char originalChar;
	string getEncoding(){
		if(parentExist)
			return parent->getEncoding() + encoding;
		else 
			return encoding;
	}
};

struct Encoding {
	int freq = 0;
	string encoding;
};

HuffJamCommand ParseInput(int argsNum, char* argv[]) {
	string command = argv[1];
	if (argsNum < 4) {
		return notcommand;
	}
	else if (command == "compress")
	{
		return compress;
	}
	else if (command == "decompress") {
		return decompress;
	}
	else {
		return notcommand;
	}
}

class HuffComparator
{
public:
	bool operator()(const HuffNode* nodeOne, const HuffNode* nodeTwo) {
		return nodeOne->val > nodeTwo->val;
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
	long charNums = 0;
	
	cout << "Calculating characters frequency" << endl;
	//Counting Freq
	vector<Encoding> Mapping(maxCharsNum);
	string inputFileContent;
	getline(InputFile, inputFileContent, (char)InputFile.eof());
	for (char& currentCharacter : inputFileContent) {
		++Mapping[unsigned int(currentCharacter)].freq;
		++charNums;
	}
	system("pause");


	cout << "Preparing The encoding" << endl;
	//Doing Initial huffman encoding
	vector<HuffNode*> DistinctNodes;
	priority_queue<HuffNode*, vector<HuffNode*>, HuffComparator> huffEncoding;
	for (int i = 0; i < Mapping.size(); ++i) {
		if (Mapping[i].freq != initialCount) {
			HuffNode* NewNode = new HuffNode();
			NewNode->val = Mapping[i].freq;
			NewNode->original = true;
			NewNode->originalChar = char(i);
			huffEncoding.push(NewNode);
			DistinctNodes.push_back(NewNode);
		}
	}

	//Optimizing the encoding
	while (huffEncoding.size() > 1) {
		auto nodeOne = huffEncoding.top();
		huffEncoding.pop();
		auto nodeTwo = huffEncoding.top();
		huffEncoding.pop();

		HuffNode* NewNode = new HuffNode;

		NewNode->rightChild = nodeOne;
		nodeOne->encoding = "1";
		nodeOne->parent = NewNode;
		nodeOne->parentExist = true;

		NewNode->leftChild = nodeTwo;
		nodeTwo->encoding = "0";
		nodeTwo->parent = NewNode;
		nodeTwo->parentExist = true;


		NewNode->val = (nodeOne->val + nodeTwo->val);
		NewNode->original = false;
		NewNode->encoding = "";

		huffEncoding.push(NewNode);
	}
	system("pause");


	//preparing file header
	cout << "Writing File Header" << endl;
	cout << DistinctNodes.size() <<endl;
	for (int i = 0; i < DistinctNodes.size(); i++){
		Mapping[int (DistinctNodes[i]->originalChar)].encoding = DistinctNodes[i]->getEncoding();
		cout << DistinctNodes[i]->originalChar << "\tgot\t" << Mapping[DistinctNodes[i]->originalChar].encoding << endl;
	}
	system("pause");


	//writing file header
	OutputFile << charNums << "\t" << DistinctNodes.size() << '\n';
	for (int i = 0; i < maxCharsNum; ++i) {
		if (Mapping[i].freq != initialCount)
			OutputFile << Mapping[i].encoding << '\t' << char(i);
	}

	//write file content
	string OutputContentBinaryString = "";
	string OutputContent = "";
	for (char& currentCharacter : inputFileContent)
	{
		OutputContentBinaryString = OutputContentBinaryString + Mapping[currentCharacter].encoding;

		if (OutputContentBinaryString.size() > 1024) {
			for (long i = 0; i < 128; ++i) {
				string feedString = OutputContentBinaryString.substr(i * 8, 8);
				reverse(feedString.begin(), feedString.end());
				bitset<8> num(feedString);
				OutputFile << char(num.to_ulong());
			}
			OutputContentBinaryString = OutputContentBinaryString.substr(1024);
		}
	}

	
	long sz = OutputContentBinaryString.size();
	long charNum = sz / 8;
	for (long i = 0; i <= charNum; ++i) {
		string feedString = OutputContentBinaryString.substr(i * 8, 8);
		reverse(feedString.begin(), feedString.end());
		bitset<8> num(feedString);
		OutputContent = OutputContent + char(num.to_ulong());
	}
	OutputFile << OutputContent;
}

void DecompressFile(ifstream& InputFile, ofstream& OutputFile) {
	int encodingSize;
	long charNums;

	InputFile >> charNums >> encodingSize;
	cout << encodingSize << endl;
	map<string, char> Mapping;

	system("pause");
	for (int i = 0; i < encodingSize; ++i) {
		string encoding;
		char seperator;
		char encodedChar;
		InputFile >> encoding;
		InputFile.get(seperator);
		InputFile.get(encodedChar);
		Mapping[encoding] = encodedChar;
		cout << Mapping[encoding] << "\t" << encoding << endl;
	}

	system("pause");
	string code = "";
	char NextCharacter;
	string OriginalContent = "";

	InputFile.get(NextCharacter);

	while (charNums > 0) {
		bitset<8> NextCharacterBits((int)NextCharacter);
		int i = 0;
		while (i < 8 && charNums > 0) {
			code = code + char(NextCharacterBits[i++] + '0');
			//cout<< code <<endl; 
			//system("pause");
			map<string, char>::iterator decodedChar = Mapping.find(code);
			if (decodedChar != Mapping.end()) {
				OriginalContent += decodedChar->second;
				//cout << decodedChar->first << "\t" << decodedChar->second <<endl;
				code = "";
				--charNums;
			}

		}
		InputFile.get(NextCharacter);
	}
	OutputFile << OriginalContent;
}

void ExecuteCommand(const HuffJamCommand & cmd, const string & InputFileName, const string & OutputFileName) {
	ifstream InputFile(InputFileName, ios::binary);
	if (InputFile.is_open()) {
		ofstream OutputFile(OutputFileName, std::ofstream::out | std::ofstream::trunc | ios::binary);
		if (OutputFile.is_open()) {
			if (cmd == compress) {
				CompressFile(InputFile, OutputFile);
			}
			else if (cmd == decompress) {
				DecompressFile(InputFile, OutputFile);
			}
			OutputFile.close();
		}
		else {
			cout << "Error Opening Output File!\n";
		}		
		InputFile.close();
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
		ExecuteCommand(cmd, argv[2], argv[3]);
	}

	//system("pause");
	return 0;
}