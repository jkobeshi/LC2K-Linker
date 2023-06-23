/**
 * Project 2
 * LC-2K Linker
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAXSIZE 300
#define MAXLINELENGTH 1000
#define MAXFILES 6

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
	char label[7];
	char location;
	int offset;
};

struct RelocationTableEntry {
	int offset;
	char inst[7];
	char label[7];
	int file;
};

struct FileData {
	int textSize;
	int dataSize;
	int symbolTableSize;
	int relocationTableSize;
	int textStartingLine; // in final executable
	int dataStartingLine; // in final executable
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry     symTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
	int textSize;
	int dataSize;
	int symTableSize;
	int relocTableSize;
};

int main(int argc, char *argv[])
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr; 
	int i, j;

	if (argc <= 2) {
		printf("error: usage: %s <obj file> ... <output-exe-file>\n",
				argv[0]);
		exit(1);
	}

	outFileString = argv[argc - 1];

	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	FileData files[MAXFILES];

  // read in all files and combine into a "master" file
	for (i = 0; i < argc - 2; i++) {
		inFileString = argv[i+1];

		inFilePtr = fopen(inFileString, "r");
		printf("opening %s\n", inFileString);

		if (inFilePtr == NULL) {
			printf("error in opening %s\n", inFileString);
			exit(1);
		}

		char line[MAXLINELENGTH];
		int sizeText, sizeData, sizeSymbol, sizeReloc;

		// parse first line of file
		fgets(line, MAXSIZE, inFilePtr);
		sscanf(line, "%d %d %d %d",
				&sizeText, &sizeData, &sizeSymbol, &sizeReloc);

		files[i].textSize = sizeText;
		files[i].dataSize = sizeData;
		files[i].symbolTableSize = sizeSymbol;
		files[i].relocationTableSize = sizeReloc;

		// read in text section
		int instr;
		for (j = 0; j < sizeText; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			instr = atoi(line);
			files[i].text[j] = instr;
		}

		// read in data section
		int data;
		for (j = 0; j < sizeData; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			data = atoi(line);
			files[i].data[j] = data;
		}

		// read in the symbol table
		char label[7];
		char type;
		int addr;
		for (j = 0; j < sizeSymbol; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d",
					label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
		}

		// read in relocation table
		char opcode[7];
		for (j = 0; j < sizeReloc; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s",
					&addr, opcode, label);
			files[i].relocTable[j].offset = addr;
			strcpy(files[i].relocTable[j].inst, opcode);
			strcpy(files[i].relocTable[j].label, label);
			files[i].relocTable[j].file	= i;
		}
		fclose(inFilePtr);
	} // end reading files
	// *** INSERT YOUR CODE BELOW ***
	//    Begin the linking process
	//    Happy coding!!!
	CombinedFiles master;
	master.textSize = 0; master.dataSize = 0;
	master.symTableSize = 0; master.relocTableSize = 0;

	int numFiles = argc - 2;
	for (int i = 0; i < numFiles; ++i) {
		files[i].textStartingLine = master.textSize;
		files[i].dataStartingLine = master.dataSize;
		for (int j = 0; j < files[i].textSize; ++j) {
			master.text[j + master.textSize] = files[i].text[j];
		}
		for (int j = 0; j < files[i].dataSize; ++j) {
			master.data[j + master.dataSize] = files[i].data[j];
		}
		for (int j = 0; j < files[i].symbolTableSize; ++j) {
			if (files[i].symbolTable[j].location != 'U') {
				if (!strcmp(files[i].symbolTable[j].label, "Stack")) {
					exit(1);
				}
				for (int k = 0; k < master.symTableSize; ++k) {
					if (!strcmp(master.symTable[k].label, files[i].symbolTable[j].label)) {
						exit(1);
					}
				}
				master.symTable[master.symTableSize] = files[i].symbolTable[j];
				if (files[i].symbolTable[j].location == 'D') {
					master.symTable[master.symTableSize].offset += files[i].dataStartingLine;
				}
				else {
					master.symTable[master.symTableSize].offset += files[i].textStartingLine;
				}
				++master.symTableSize;
			}
		}
		for (int j = 0; j < files[i].relocationTableSize; ++j) {
			master.relocTable[j + master.relocTableSize] = files[i].relocTable[j];
			if (!strcmp(files[i].relocTable[j].inst, ".fill")) {
				master.relocTable[j + master.relocTableSize].offset += files[i].dataStartingLine;
			}
			else {
				master.relocTable[j + master.relocTableSize].offset += files[i].textStartingLine;
			}
		}
		master.textSize += files[i].textSize;
		master.dataSize += files[i].dataSize;
		master.relocTableSize += files[i].relocationTableSize;
	}
	for (int i = 0; i < master.relocTableSize; ++i) {
		if (!strcmp(master.relocTable[i].label, "Stack")) {
			if (!strcmp(master.relocTable[i].inst, ".fill")) {
				master.data[master.relocTable[i].offset] += (master.textSize + master.dataSize);
			}
			else {
				master.text[master.relocTable[i].offset] += (master.textSize + master.dataSize);
			}
		}
		else if (isupper(master.relocTable[i].label[0])) {
			int j = 0;
			while (j < master.symTableSize) {
				if (!strcmp(master.symTable[j].label, master.relocTable[i].label)) {
					break;
				} ++j;
			}
			if (j >= master.symTableSize) {
				exit(1);
			}
			int pos = master.symTable[j].offset;
			if (master.symTable[j].location == 'D') {
				pos += master.textSize;
			}
			if (!strcmp(master.relocTable[i].inst, ".fill")) {
				master.data[master.relocTable[i].offset] >>= 16;
				master.data[master.relocTable[i].offset] <<= 16;
				master.data[master.relocTable[i].offset] += pos;
			}
			else {
				master.text[master.relocTable[i].offset] >>= 16;
				master.text[master.relocTable[i].offset] <<= 16;
				master.text[master.relocTable[i].offset] += pos;
			}
		}
		else {
			int loc = master.data[master.relocTable[i].offset];
			loc >>= 16; loc <<= 16;
			loc = master.data[master.relocTable[i].offset] - loc;
			if (strcmp(master.relocTable[i].inst, ".fill")) {
				loc = master.text[master.relocTable[i].offset];
				loc >>= 16; loc <<= 16;
				loc = master.text[master.relocTable[i].offset] - loc;
			}
			int pos = files[master.relocTable[i].file].textStartingLine;
			if (loc >= files[master.relocTable[i].file].textSize) {
				pos = master.textSize - files[master.relocTable[i].file].textSize + files[master.relocTable[i].file].dataStartingLine;
			}
			if (!strcmp(master.relocTable[i].inst, ".fill")) {
				master.data[master.relocTable[i].offset] += pos;
			}
			else {
				master.text[master.relocTable[i].offset] += pos;
			}
		}
	}
	for (int i = 0; i < master.textSize; ++i) {
		fprintf(outFilePtr, "%d\n", master.text[i]);
	}
	for (int i = 0; i < master.dataSize; ++i) {
		fprintf(outFilePtr, "%d\n", master.data[i]);
	}

} // main
