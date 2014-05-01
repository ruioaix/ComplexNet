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
* struct LineFile is related structure.
* struct LineFile * lf = create_LineFile(char * filename, x, x, x, -1);
* files are read line by line.
* each line contains serval parts which are separated with space, ":", ",", "\t".
* a line can contain 45 parts at most(9 int, 9 double, 9 char, 9 long, 9 char point[I mean string]).
* lf->i1 is int array, lf->d1 is double array, lf->c1, lf->l1, lf->cc1...
* lf->linesNum is the number of lines in a file.
* lf->memNum is the size of memory which have been allocated.
* lf->memNum >= lf->linesNum.
* if a file named "xx" contain 5 parts in each line, the first one is int, second is double, third is char, forth is long, fifth is char * ,
	then struct LineFile * lf = create_LineFile("xx", 1, 2, 3, 4, 5, -1) is the right usage.
	and lf->i1, lf->d1, lf->c1, lf->l1, lf->cc1 will be used to store the data.
* 1 means int type, 2 means double, 3 means char, 4 means long, 5 means char * .
* -1 means stop.
* if you use lf = create_LineFile("xx", 1, 2, -1), then only the first two parts of the file are read into struct LineFile.
* if you use lf = create_LineFile("xx", 1, 1, 1, -1), then the first three parts will be read as int type.
* if you use lf = create_LineFile("xx", 1, 2, 3, 4, 5, 1, -1), then lf->i2 will be filled with -1.
* if you use lf = create_LineFile("xx", 1, 2, 3, 4, 5, 2, -1), then lf->d2 will be filled with -1.0.
* if you use lf = create_LineFile("xx", 1, 2, 3, 4, 5, 5, -1), then lf->cc2 will be filled with NULL.
* More information in src/linefile.h&c

###Generate certain network
* struct LineFile is related structure.
* struct LineFile * lf = line1d_DS(int N, enum CICLENET cc, enum DIRECTNET dd);
	this will create a dataset of line(maybe ring if cc==CICLE), directed or undirected.
	the dataset will be in lf, if you want to write it into a file, use print_LineFile(lf, "a name of file");
* struct LineFile * lf = lattice2d_DS(int L, enum CICLENET cc, enum DIRECTNET dd);
	this will create a dataset of lattice, cicle or non-cicle, directed or undirected.
	the dataset will be in lf, if you want to write it into a file, use print_LineFile(lf, "a name of file");
* More information in src/dataset.h&c

###Pseude random number generater
* I use Mersenne Twister.
* for example, int r = get_i31_MTPR(), r will get a 31bits prn(I assume int is 32bit, so r is no problem).
	r= get_i31_MTPR(), you get another 31bits prn.
* you don't need to set seed before using get_i31_MTPR(), I seed a default seed already. 
	but if you use the default seed, then you always get the same prn sequence.
* you can set seed with set_seed_MTPR(unsigned long seed).
* you also can use set_timeseed_MTPR(void), this will use the unix timestamp as seed.
* More information in src/mtprand.h&c

##Network

###unweighted and undirected network
* struct iiNet is related structure.
* struct iiNet * net = create_iiNet(struct LineFile * lf);
* only lf->i1 and lf->i2 will be used. 
	for example: lf->i1[23] == 44 and lf->i2[23] == 554, then there is a link between 44 and 554.
* net->maxId is the maximum id of node.
* net->minId is the meanful minimum id of node, of course, net->minId >= 0.
* net->count[34] is the degree of node 34.
* the length of net->count is (net->maxId + 1), so net->count[net->maxId] is the last element.
* net->idNum is the number of meanful ids. the meanful id is the id of node which has at least one link.
* net->edgesNum is the number of edges. (...ok, it's obvious)
* net->countMax is the max degree in network.
* net->countMin is the min degree in network.
* net->edges[23][33] == 174, then there is a link between 23 and 174. (174 is the 34th linked id of 23)
* struct LineFile * lf, lf->i1[11] == 23, lf->i2[11] == 174 and lf->i1[33] == 23, lf->i2[33] == 174, this is a possible situation.
	because LineFile does not care about what is in one line of files, it just read line by line.
	then, if this lf is used to create struct iiNet * net, the net is a problem.
	make sure the lf is profect, and this is your and my responsibility.
* information is stored twice in the iiNet, if there is net->edges[23][33] == 174, there will be net->edges[174][xx] = 23.
	this is a choice.
* More information in src/iinet.h&c

###weighted and undirected network
* struct iidNet is related structure.
* it's very similar to struct iiNet, only contains a net->d member more than iiNet.
* struct iidNet * net = create_iidNet(struct LineFile * lf);
* lf->i1 and lf->i2 and lf->d1 will be used.
	for example: lf->i1[23] == 44 and lf->i2[23] == 554 and lf->d1[23] = 3.14, then there is a link between 44 and 554, and there is 
	property on this link, maybe weight, maybe time, and it's 3.14.
* all others is same with the previous iiNet.
* though the title of this section is weighted and undirected network, but the net->d can be used to represent a lot of things.
* More information in src/iidnet.h&c

###bipartite network
* struct Bip is related structure.
* it's very similar to struct iiNet, only contains a bip->score member more thean iiNet.
* struct Bip * bip = create_Bip(struct LineFile * lf, int index).
* lf->i1 and lf->i2 will be used, lf->i3 may be used.
	the research on bipartite network basicly focus on recommendation, in most casees, we don't conside about the score which a user
	gives to a item. but I have a paper about improving the score of recommendation result. so I add the bip->score.
* Bip is different from iiNet, if bip->edges[23][33] == 174, it means the user whose id is 23 chooses the item whose id is 174.
	23 and 174 is not in same id-space. but for iiNet, 23 and 174 is in the same id-space.
* when you use create_Bip(lf, 1), you create a Bip in which bip->count[23] == 45 means the degree of 23 user is 45.(I assume lf->i1 is the user, i2 is item).
	means the 23 user chooses 45 items.
* when you use create_Bip(lf, 2), you create a Bip in which bip->count[23] == 45 means the degree of 23 item is 45.(I assume lf->i1 is the user, i2 is item).
	means the 23 item is choosed by 45 users.
* in most casees, you need to create two bips in order to get all detail information about a bipartite network.
* More information in src/bip.h&c
