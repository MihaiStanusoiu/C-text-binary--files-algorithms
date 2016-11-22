#include <stdio.h>

/*
	Function gets a file stream and a position.
	Returns position of start of the line.
	Input:
		- fstream : FILE
		- pos : int
	Output:
		- int
*/
int GetIndexStartOfLine(FILE *fstream, int pos)
{
	char c;
	int status;
	int initpos;

	if (NULL == fstream)
	{
		printf("In GetIndexStartOfLine: fstream not allocated");
		return -1;
	}

	initpos = ftell(fstream);	//		Save initial position in file

	fseek(fstream, pos, SEEK_SET);	//		Position is set on end of line

	c = fgetc(fstream);
	if (c == '\n')	//	  If position is on end of line, move back position
	{
		fseek(fstream, -2, SEEK_CUR);
	}

	//	Proceed to search for \n (end of previous line)
	do
	{
		status = fseek(fstream, -1, SEEK_CUR);	//	Move position back by one byte
		if (status != 0)	//	Treat error case
		{
			perror("Unsuccessful fseek\n");
			return -1;
		}

		pos = ftell(fstream);
		if (pos == 0)	//	If position is at the beginning of the file, return position
		{
			return pos;
		}

		c = fgetc(fstream);		//	Get character from file

		status = fseek(fstream, -1, SEEK_CUR);		// Move back position, due to fgetc moving the position to the right
	} while (c != '\n');

	fseek(fstream, initpos, SEEK_SET);	//	 Set position back to initial value

	return pos + 1;		// Return position
}

/*
	Function writes current line from input file fin to output file fout
*/
int WriteLineToFile(FILE *fin, FILE *fout)
{
	char c;

	if (NULL == fin || NULL == fout)	
	{
		printf("WriteLine: arguments invalid\n");
		return -1;
	}

	do
	{
		c = fgetc(fin);		// Get character from input file
		if (EOF == c)	//	Reached end of input file
		{
			c = '\n';
		}

		fputc(c, fout);		// Write character to output file
	} while (c != '\n');

	return 0;
}


/*
	Main gets input text filename as command line argument
*/
int main(int argc, char const *argv[])
{
	//	Data zone
	
	char newFileName[256];	//	char[] to store new file name
	int pos;	//	Variable used to store position in input file
	int currPos;
	int returnStatus;	//	Stores return status
	int startPos;	//	Stores position of beginning of input line
	FILE *f, *g;	//	Input and output files descriptors

	//	Preinitialisation zone

	f = g = NULL;
	returnStatus = 0;

	//	Solution

	if (argc < 2)	//	main must receive input filename as argument
	{
		perror("Insufficient number of arguments!\n");
		return -1;
	}

	f = fopen(argv[1], "r");
	if (NULL == f)	//	File is not successfully opened
	{
		perror("Error at opening file %s\n", argv[1]);
		return -1;
	}

	//	Build new file name string
	strcpy(newFileName, argv[1]);
	strcat(newFileName, ".inv");

	g = fopen(newFileName, "w");
	if (NULL == g)	//	File is not successfully opened
	{
		perror("Error at opening file %s\n", argv[1]);
		returnStatus = -1;
		goto CleanUp;
	}

	printf("Output file created!\n");

	//	Proceed to write lines in reverse order
	fseek(f, 0, SEEK_END);
	pos = ftell(f);
	
	while (pos > 0)		//	Process lines
	{
		startPos = GetIndexStartOfLine(f, pos);		//	Get position of start of current line
		fseek(f, startPos - pos, SEEK_CUR);		//	Move position to beginning of line
		WriteLineToFile(f, g);		//	Write line to output file

		pos = startPos - 1;
	}	// Go to next line

CleanUp:
	//Close files handles

	fclose(f);
	fclose(g);

	return returnStatus;
}