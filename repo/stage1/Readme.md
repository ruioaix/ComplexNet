# scope of usage of this program 
TOO OLD, I will rewrite this doc sometime later. 
 - rui 20131218.

## structure
- main
- src
-- src/compact
--- bip2.c
--- bip3i.c
--- bipartite.c
--- dnet.c
--- dtnet.c
--- iidnet.c
--- net.c
-- src/hashtable
--- dmp.c
-- src/linefile
--- i3dlinefile.c
--- i3linefile.c
--- i4linefile.c
--- i5dslinefile.c
--- i5linefile.c
--- i5sslinefile.c
--- i7ddslinefile.c
--- iid3linefile.c
--- iidlinefile.c
--- iilinefile.c
--- ilinefile.c
--- innlinefile.c
-- src/matrix
--- pspipr.c
--- snapshot.c
-- src/utility
--- bitsign.c
--- error.c
--- hashtable.c
--- random.c
--- sort.c
--- threadpool.c
--- threadsafe_random.c
-- Main
--- Main/xx
-- inc
--- inc/xx (same as src);


## complexnet_file
1, in complexnet_file.h, there are many structures with similar name. like: iiLineFile, innLineFile, i5sdLineFIle.
2, for now: 20130830, I got 8 kinds. ii means the file contains two parts(two int) in a line, all the line which not contains two int will be ignored. inn means unlimit int in one line. i4 means four int in one line. idi means int-double-int, i5sd means five int and a c string and a double. ...
	1, iiLine, iiLineFile
	2, innLine, innLineFile
	3, i4Line, i4LineFile
	4, idiLine, idiLineFile
	5, i5sdLine, i5sdLineFIle
	6, iidiLine, iidiLineFile
	7, i5ssLine, i5ssLineFile
	8, i7ddsLine, i7ddsLineFile
3, all these structures have two similar functions: 
	1, void free_i5ssLineFile(struct i5ssLineFile *file);
	2, struct i5ssLineFile *create_i5ssLineFile(const char *const filename);
4, when I get a file, I always can add a new structure and two new functions. basically, what I need to do is copy and small change.
5, the disadvantage is too much structure and too much functions, but I can tolerate that.
6, function create_xxxLineFile.
	* input: filename.
	* output: a point to a struct xxxLineFile object.
	* the output will be used by some net build function in complexnet_xxxnet file.
* vertex id is not required to start at 0, and vertex id is not required to be continuous.
	* but, the memory used by net is according to the vertex max id.
	* "start at 0" and "continuous" can make sure the memory usage is minimal.
	* in complexnet_hashtable, you can find a function which can make vertex id "start at 0" and "continuous", for certain xxxLineFile structure, maybe you need to create a new similar function, though you need to do some copy work and small change.
* delimiter can be set as you need, the most popular delimiters are "\t"/","/space/"\r"/"\n".

## complexnet_xxxnet
* dnet means direct net, without temporal, without weight.
* dtnet means direct temporal net, without weight.
* net means net, without direction, without temporal, without weight.
* the rule is that if a net has a attribute, the name will display the attribute with a char.
* the net build function name will start with create_, end with the something like DirectNet.
* all complexnet_xxnet will contains a static global net object, this can make function calls much more simple. and I believe this make sence: you will not change a net in most case.
* but if you really want do some change to a net, the clonexxNet functions are supplied. (in most case, net is the largest usage of memory, so I do not think cloneNet is good practice.

## complexnet_random (MT PRNG)
* mersenne twrister random generater is used. 
* before using mt prng, you should init the mt prng with function: void init_by_array_MersenneTwister(unsigned long init_key[], int key_length); this is suggested.
* the mt prng I got from http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html is not thread safe, so I change it to be threadsafe.
* in multi-thread enviorment, you should call function: void init_MersenneTwister(void); first to init a mutex and something else.
	* in each thread which you want use mt prng, you need to call function: int init_by_array_MersenneTwister_threadsafe(unsigned long init_key[], int key_length);
	* the function will return a int variable "t", then you can use unsigned long genrand_int32_threadsafe(int t); or double genrand_real1_threadsafe(int t); to generate random number.
	* at the end of thread, you should relese the mt prng you create before with calling function: void free_MersenneTwister_threadsafe(int t); this function will set "t" to be valueable for other thread.
	* a sad thing is there is a max number of  thread-safe mt prng: define Thread_Safe_MAX_MersenneTwister 100.
	* the good thing is in most case you will not have more than 100 threads at a time. if you will create more than 100 threads ,but at a time the total number of threads do not exceed 100, it is fine. 
	* of course, you can set Thread_Safe_MAX_MersenneTwister to 200 or larger, the limit is the size of stack on your mechine.
* the thread-safe mt prng is as fast as the origin mt prng, because I use space(stack space) to exchange speed. actually, that is why there is Thread_Safe_MAX_MersenneTwister.

## complexnet_threadpool
* threadpool will be the base of development later.
* TODO

## complexnet_hashtable
## complexnet_sort
## complexnet_error
TODO
