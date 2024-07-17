#include <stdio.h>
#include <stdlib.h>

char* load_file(char* path) {
	
	// open file at path and load it into file object
	FILE* fp = fopen(path, "r");

	// if loaded file is NULL
	if(fp == NULL) {
		// print error message and return NULL
		printf("ERROR: Failed to read file at path: `%s`.", path);
		return NULL;
	}

	// variable to hold size of file in characters
	int size;

	// seek to the end
	fseek(fp, 0, SEEK_END);

	// set size to be equivalent to position
	size = ftell(fp);

	// seek back to beginning
	fseek(fp, 0, SEEK_SET);

	// create string on stack with specific size
	char* stringVersion = calloc(size+1, sizeof(char));

	// char to store current character
	char ch;

	// index of character
	int index = 0;

	// iterate thru all characters in file
	while((ch = fgetc(fp)) != EOF) {
		// append character to stringVersion
		stringVersion[index] = ch;

		// increment index
		index++;
	}

	// append \0 to end
	stringVersion[index] = '\0';

	// close file reading at the end
	fclose(fp);

	// return allocated string (REMEMBER TO FREE FROM HEAP LATER)
	return stringVersion;

}
