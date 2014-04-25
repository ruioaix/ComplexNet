#programs to deal with all kinds of simulations and calculations in complex network.
In complex network, there are two concepts which always available:
1. node/vertex/point
2. edge/line

##Limitation
* *int* type is used to describ the node's id&number. 
	* I assume *int* type has 32bits at least, of course, this is not the Standard C's request.
	* if on your mechine, the *int* type only has 16bits, the correctness of the program cannot be guaranteed.
* *long* type is used to describ the edgs's id&number.
	* I assume *long* type has 64bits at least.
* the minimum value of node's id is 0, the maximum value of node's id is 2,147,483,647 (2^31 - 1). \
	the ids don't need to be continuous, but the program will be faster and more spacesaving if it's continuous.\
	BTW, though 2,147,483,647 is available for nodeid, but you may not want to use it because of PC's memory.\
	2^31 * 4Bytes = 16GB. I don't think modern PC(2014) can handle this.\
	the memory usage and the max id of nodes is in the direct ratio.\
	if you have a network only containing 0 and 2147483647, the memory usage is also 16GB.\
	keep node ids continuous is always a good idea. (there is a program here can serialize data set).
* the minimum value of lines' number is 1, the maximum is 2^63 - 1.\
	if the linesnumber of your dataset exceed one billion, it should not be PC's work.
