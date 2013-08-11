# scope of usage of dnet program
## read file 
### dnet file
* function readFileLBL.
	* input: filename.
	* output: a point to a struct NetFile object.
	* the output will be used by buildDNet.
* vertex id is not required to start at 0, and vertex id is not required to be continuous.
	* but, the memory used by net is according to the vertex max id.
	* "start at 0" and "continuous" can make sure the memory usage is minimal.
* for dnet, a line of dnet file should be composed by integer, "\t", ",", space, "\r", "\n".
	* for dnet, a line contains two parts. the first part is the begin vertex id, the other is the end vertex id.
	* "\t",",",space will be treated as delimiter. that means these three characters can exist before or after the integer.
	* "\r","\n" can exist at the end of a line.
	* any other kinds of line will stop the program, and throw a wrong message.
* empty file will make the program stop, and throw a wrong message.
* a file with a empty line will act as same as the empty file.

### infectsource file
* function readISfromFile.
	* input: filename.
	* output: a point to a struct InfectSource object.
	* the output will be used by  buildIStoDNet.
* a line of infectsource file should be composed by integer, "\t", ",", space, "\r", "\n".
	* a line only contains one part: a infectsource integer.
	* "\t",",",space will be treated as delimiter. that means these three characters can exist before or after the integer.
	* "\r","\n" can exist at the end of a line.
	* the integer in each line should be a vertex id in net. if not, the function buildIStoDNet will throw a wrong message and stop program.
	* any other kinds of line will stop the program, and throw a wrong message.
* empty file will make the program stop, and throw a wrong message.
* a file with a empty line will act as same as the empty file.

## build net
* function buildDNet.
	* input: a point to a struct NetFile object.
	* output: a point to a struct DirectNet object.
	* the DirectNet struct will be used to spread and other net work.

## spread
* function buildIStoDNet.
	* this function is used inside spread_touch_all and other spread function.
	* input: a point to a struct InfectSource object, a point to a struct DirectNet object.
	* no output.
	* the function set the IS to the NET.
	* the function will check whether InfectSource vertex exists in the net, if not exist, the vertex will be ignored, throw a message.

* function spread_touch_all.
	* input: InfectSource point, DirectNet point, infectRate.
	* output: spread number.

* after spread, all the information can be fetched from the DirectNet struct. do that in main.c

## main.c
* read net file. readFileLBL
* read InfectSource file. readISfromFile
* build net.  buildDNet
* begin spread. spread_touch_all or spread_touch_part
* anaylsis net. whatever you want.
