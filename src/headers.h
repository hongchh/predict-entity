#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>
using namespace std;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

const float FLOATZERO = 1e-6F;
const float PI = 3.141592654F;
const float MINFREQ = 0.5;          // parameter of the medrank algorithm
const int NODE_ENTRY_SIZE = 125;    // number of entry in a b+ tree node
const int PAGE_SIZE = 1024;         // size of a page
const int LINE_NUM = 50;            // number of the projection lines
const int MIN_PAGE_SIZE = 1024;     // minimum size of a page
const int CHAR_SIZE = 1;            // size of a char variable, 1 byte

#include "BlockFile.h"
#include "DataStructure.h"
#include "BTree.h"