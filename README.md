#programs to deal with all kinds of calculations in complex network.
In complex network, there are two concepts which always available:
	1. node/vertex/point
	2. edge/line

##Limitation
* *int* type is used to describ the node's id&number. 
	* I assume *int* type has 32bits at least, of course, this is not the Standard C's request.
	* if on your mechine, the *int* type only has 16bits, the correctness of the program cannot be guaranteed.
* *long* type is used to describ the edgs's id&number.
	* I assume *long* type has 64bits at least.
* the minimum value of node's id is 0.
* the maximum value of node's id is 2,147,483,647(2^31 - 1).
