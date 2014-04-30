#programs to deal with all kinds of simulations and calculations in complex network.
In complex network, there are two concepts which always available:
1. node/vertex/point
2. edge/line

##Limitation
* **int** type is used to describ the node's id&number. 
	* I assume **int** type has 32bits at least, of course, this is not the Standard C's request.
	* if on your mechine, the **int** type only has 16bits, the correctness of the program cannot be guaranteed.
* **long** type is used to describ the edgs's id&number.
	* I assume **long** type has 64bits at least.
* the minimum value of node's id is 0, the maximum value of node's id is 2,147,483,647 (2^31 - 1). 
	* the ids don't need to be continuous, but the program will be faster and more spacesaving if it's continuous.
	* BTW, though 2,147,483,647 is available for nodeid, but you may not want to use it because of the limitation of PC's memory.
	* 2^31 * 4Bytes = 16GB. I don't think modern PC(2014) can handle this.
	* the memory usage and the max id of nodes is in the direct ratio.
	* if you have a network only containing two nodes: 0 and 2147483647, the memory usage is also 16GB.
	* keeping node ids continuous is always a good idea. (there is a program here can serialize data set).
* the minimum value of lines' number is 1, the maximum is 2^63 - 1.
	* but if the linesnumber of your dataset exceed one billion, it should not be PC's work, put them on a cluster.

##Functionality

###the way to read file
* struct LineFile is the most basic structure.
* struct LineFile * lf = create_LineFile(char * filename, x, x, x, -1);
* files are read line by line.
* each line contains serval parts which are separated with space, ":", ",", "\t".
* a line can contain 45 parts at most(9 int, 9 double, 9 char, 9 long, 9 char point[I mean string]).
* lf->i1 is int array, lf->d1 is double array, lf->c1, lf->l1, lf->cc1...
* lf->linesNum is the number of lines in a file.
* lf->memNum is the size of memory which have been allocated.
* lf->memNum >= lf->linesNum.
* if a file named "xx" contain 5 parts, the first one is int, second is double, third is char, forth is long, fifth is char * ,
	then struct LineFile * lf = create_LineFile("xx", 1, 2, 3, 4, 5, -1).
* 1 means int type, 2 means double, 3 means char, 4 means long, 5 means char * .
* -1 means stop.
* if you use lf = create_LineFile("xx", 1, 2, -1), then only the first two parts of the file are read into struct LineFile.
* if you use lf = create_LineFile("xx", 1, 1, 1, -1), then the first three parts will be read as int type.
