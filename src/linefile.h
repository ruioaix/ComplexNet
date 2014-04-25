/**
 * this file is used to read text file into struct LineFile.
 * struct LineFile has its limit. for now, at most, it can process a line with 9 int and 9 double.
 * lines will be seprated into parts with "\t", space, ":", "\n", ",".
 *
 * when you want to read a file, just use create_LineFile, with right parameters.
 * when you use a struct LineFile, you should have a purpose.
 * 	for example, use lf(a struct LineFile object) to create a iidNet, then you should use i1,i2,d1.
 * 		then you should check lf->i1, lf->i2, lf->d1 != NULL.
 * 			if any one of these three is NULL, there is something wrong.
 *
 * when you get a file contains only 2 parts in a line, but you use create_LineFile(xx, 1, 1, 1, -1);
 * 	then you get -1 for each i3[x];
 *
 * when you get a file contains 3 parts in a line, but you use create_LineFile(xx, 1, 1, -1);
 * 	then you only fetch the first 2 parts from each line. lf->i3 will be NULL.
 *
 *  Author: RuiXiao <xrfind@gmail.com>
 */

#ifndef CN_LINEFILE_H
#define CN_LINEFILE_H

struct LineFile {
	int *i1;
	int *i2;
	int *i3;
	int *i4;
	int *i5;
	int *i6;
	int *i7;
	int *i8;
	int *i9;
	double *d1;
	double *d2;
	double *d3;
	double *d4;
	double *d5;
	double *d6;
	double *d7;
	double *d8;
	double *d9;

	long linesNum;
	long memNum;

	char *filename;
};

/**
 * create_LineFile("fileA", 1, 2, 1, 2, -1) means: 
 * 	for each line in fileA, only read the first four parts into struct.
 * 	the arg "1" means: the first and the third part is an int.
 * 	the arg "2" means: the second and the fourth part is a double.
 * 	the arg "-1" is the guard to let function know the last argument's position.
 *
 * if the number of the parts in one line is less than 4,
 * 	than the line is ignored and a warning will be sent to stdout.
 * if more than 4, only 4 parts is read in.
 */
struct LineFile *create_LineFile(char * filename, ...);
struct LineFile *init_LineFile(void);

struct LineFile *add_LineFile(struct LineFile *lf1, struct LineFile *lf2);

/// print the struct LineFile's content into a file
void print_LineFile(struct LineFile *lf, char *filename);

/// free;
void free_LineFile(struct LineFile *lf);

#endif
