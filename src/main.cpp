#include "headers.h"

/* check if string <a> is the same as string <b> */
bool isSameString(const char* a, const char* b) {
    int length_a = strlen(a);
    int length_b = strlen(b);
    if (length_a != length_b) return false;
    for (int i = 0; i < length_a; ++i) {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

/* convert a string to a integer
 * eg. "123" -> 123 */
int stringToInt(const char* str) {
    int length = strlen(str);
    int num = 0;
    for (int i = 0; i < length; ++i) {
        num = num * 10 + (str[i] - '0');
    }
    return num;
}

int main(int argc, char* argv[]) {
    char* dataFile  = NULL;
    char* queryFile = NULL;
    int*  index_m   = NULL;
    int*  index_b   = NULL;
    int   dataSize  = 0;
    int   querySize = 0;
    int   dimision  = 0;

    // parameters from command line
    if (argc != 11) {
        printf("parameter error.\n");
        return 1;
    }
    for (int i = 1; i < argc; i += 2) {
        if (isSameString(argv[i], "-n")) {
            dataSize = stringToInt(argv[i+1]);
        } else if (isSameString(argv[i], "-d")) {
            dimision = stringToInt(argv[i+1]);
        } else if (isSameString(argv[i], "-qn")) {
            querySize = stringToInt(argv[i+1]);
        } else if (isSameString(argv[i], "-ds")) {
            dataFile = new char[strlen(argv[i+1]) + 1];
            strcpy(dataFile, argv[i+1]);
        } else if (isSameString(argv[i], "-qs")) {
            queryFile = new char[strlen(argv[i+1]) + 1];
            strcpy(queryFile, argv[i+1]);
        }
    }
    
    // training data
    Vector* data = new Vector[dataSize];
    for (int i = 0; i < dataSize; ++i)
        data[i].init(dimision);
    // query data
    Vector* query = new Vector[querySize];
    for (int i = 0; i < querySize; ++i)
        query[i].init(dimision);
    // b+ trees reference
    BTree* trees = new BTree[LINE_NUM];
    // project value of a query point
    float* queryProjectVal = new float[LINE_NUM];

    Util u(dataFile, queryFile, dataSize, LINE_NUM, querySize, dimision);

    printf("[Main] preprocessing ...\n");
    int t = clock();
    u.preprocess(trees, PAGE_SIZE, data);
    t = clock()-t;
    printf("[Main] total preprocess time: %fs.\n", ((float)t)/CLOCKS_PER_SEC);

    t = clock();
    u.getQueryData(query);
    t = clock()-t;
    printf("[Main] read query data time: %fs.\n", ((float)t)/CLOCKS_PER_SEC);

    // predicting
    MedRank m(dataSize, LINE_NUM, TOP_N_CLOSEST_POINT, MINFREQ);
    index_m = new int[TOP_N_CLOSEST_POINT];
    index_b = new int[TOP_N_CLOSEST_POINT];

    FILE* out = fopen("./data/detail.txt", "w");
    int totalMedrankQueryTime = 0;
    int totalBFQueryTime = 0;
    int totalProbNum = 0;
    int m_hit = 0, b_hit = 0, probNum;

    printf("[Main] predicting ...\n");
    for (int i = 0; i < querySize; ++i) {
        // project a query point on 50 lines, save the project value in <queryProjectVal>
        u.project(query[i], queryProjectVal);
        
        // use the b+ tree index and <queryProjectVal> to find
        // save the number of accessing pages in <probNum>
        probNum = 0;
        t = clock();
        m.medrank(trees, queryProjectVal, probNum, index_m);
        totalMedrankQueryTime += clock() - t;
        totalProbNum += probNum;

        // find by bf
        t = clock();
        m.findClosestPoint(data, query[i], index_b);
        totalBFQueryTime += clock() - t;

        // predict type (entity)
        int type_m = u.predict(data, index_m, TOP_N_CLOSEST_POINT);
        int type_b = u.predict(data, index_b, TOP_N_CLOSEST_POINT);
        fprintf(out, "[Query %3d] %d (medrank), %d (bf), %d(real).\n", i+1, type_m, type_b, query[i].type);
        if (type_m == query[i].type) ++m_hit;
        if (type_b == query[i].type) ++b_hit;
    }
    fclose(out);

    // print experiment result
    printf("[Main] total medrank query time: %fs\n", ((float)totalMedrankQueryTime)/CLOCKS_PER_SEC);
    printf("[Main] average medrank query time: %fs\n", ((float)totalMedrankQueryTime/querySize)/CLOCKS_PER_SEC);
    printf("[Main] total bf query time: %fs\n", ((float)totalBFQueryTime)/CLOCKS_PER_SEC);
    printf("[Main] average bf query time: %fs\n", ((float)totalBFQueryTime/querySize)/CLOCKS_PER_SEC);
    printf("[Main] average probNum per line: %f\n", (float)totalProbNum/querySize/LINE_NUM);
    printf("[Main] medrank hit ratio: %f\n", (float)m_hit / querySize);
    printf("[Main] bf hit ratio: %f\n", (float)b_hit / querySize);
    printf("[Main] please checkout ./data/detail.txt for more detail\n");

    // clear resources
    delete [] dataFile;
    delete [] queryFile;
    delete [] index_m;
    delete [] index_b;
    delete [] trees;
    delete [] data;
    delete [] query;
    delete [] queryProjectVal;

    return 0;
}
