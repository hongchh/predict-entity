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
    int*  index     = NULL;
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

    MedRank m(dataSize, LINE_NUM, TOP_N_CLOSEST_POINT, MINFREQ);
    index = new int[TOP_N_CLOSEST_POINT];

    FILE* out = fopen("./data/detail.txt", "w");
    int totalQueryTime = 0, totalProbNum = 0, totalFindTime = 0;
    float totalRatio = 0.0;
    printf("[Main] medranking ...\n");
    for (int i = 0; i < querySize; ++i) {
        // project a query point on 50 lines, save the project value in <queryProjectVal>
        u.project(query[i], queryProjectVal);
        
        int probNum = 0;
        t = clock();
        // use the b+ tree index and <queryProjectVal> to find a approximate closest point
        // save the number of accessing pages in <probNum>
        m.medrank(trees, queryProjectVal, probNum, index);
        totalQueryTime += clock() - t;
        totalProbNum += probNum;

        // output the medrank result
        fprintf(out, "[Query %3d (approximate)]: \n", i+1);
        for (int j = 0; j < TOP_N_CLOSEST_POINT; ++j) {
            fprintf(out, "%6d", index[j]);
        }
        fprintf(out, "\n");

        t = clock();
        // find the real closest point
        m.findClosestPoint(data, query[i], index);
        totalFindTime += clock() - t;

        // output the real top N closest points
        fprintf(out, "[Query %3d (real)]: \n", i+1);
        for (int j = 0; j < TOP_N_CLOSEST_POINT; ++j) {
            fprintf(out, "%6d", index[j]);
        }
        fprintf(out, "\n");

        // TODO
        // calculate overall ratio
    }
    fclose(out);

    // printf("[Main] average overall ratio: %f\n", totalRatio/querySize);
    printf("[Main] total query time: %fs\n", ((float)totalQueryTime)/CLOCKS_PER_SEC);
    printf("[Main] average query time: %fs\n", ((float)totalQueryTime/querySize)/CLOCKS_PER_SEC);
    printf("[Main] total time for finding closest point: %fs\n", ((float)totalFindTime)/CLOCKS_PER_SEC);
    printf("[Main] average time for finding closest point: %fs\n", ((float)totalFindTime/querySize)/CLOCKS_PER_SEC);
    printf("[Main] average probNum per line: %f\n", (float)totalProbNum/querySize/LINE_NUM);
    printf("[Main] please checkout ./data/detail.txt for more detail\n");

    delete [] dataFile;
    delete [] queryFile;
    delete [] index;
    delete [] trees;
    delete [] data;
    delete [] query;
    delete [] queryProjectVal;

    return 0;
}
