#include "headers.h"

Util::Util(const char* dataFile, const char* queryFile,
           const int& dataSize, const int& randomVectorNum,
           const int& querySize, const int& dimision) {
    this->dataSize = dataSize;
    this->querySize = querySize;
    this->randomVectorNum = randomVectorNum;
    this->randomVec = new Vector[randomVectorNum];
    this->dataFile = new char[strlen(dataFile) + 1];
    this->queryFile = new char[strlen(queryFile) + 1];
    strcpy(this->dataFile, dataFile);
    strcpy(this->queryFile, queryFile);
    // initialize the dimision of the random unit vector
    for (int i = 0; i < randomVectorNum; ++i) {
        randomVec[i].init(dimision);
    }
}

Util::~Util() {
    if (randomVec != NULL)
        delete [] randomVec;
    if (dataFile != NULL)
        delete [] dataFile;
    if (queryFile != NULL)
        delete [] queryFile;
}

float Util::normalDistribution() {
    float u1, u2;
    do {
        u1 = 0.0f + (1.0f - 0.0f) * ((float)rand()) / ((float)RAND_MAX);
    } while (u1 < FLOATZERO);
    u2 = 0.0f + (1.0f - 0.0f) * ((float)rand()) / ((float)RAND_MAX);
    float x = 0.0 + 1.0 * sqrt(-2.0f * log(u1)) * cos(2.0f * PI * u2);
    // float x = 0.0 + 1.0 * sqrt(-2.0f * log(u1)) * sin(2.0f * PI * u2);
    return x;
}

void Util::preprocess(BTree* trees, const int& pageSize, Vector* data) {
    // generate random unit vector
    srand(time(NULL));
    for (int k = 0; k < randomVectorNum; ++k) {
        float length = 0.0;
        for (int i = 0; i < randomVec[k].dimision; ++i) {
            randomVec[k].dim[i] = normalDistribution();
            length += randomVec[k].dim[i] * randomVec[k].dim[i];
        }
        // unitfy
        length = sqrt(length);
        for (int i = 0; i < randomVec[k].dimision; ++i) {
            randomVec[k].dim[i] /= length;
        }
    }

    /* read vector from Mnist.ds
     * using "%d" to read data is faster than "%f" */
    FILE* in = fopen(dataFile, "r");
    if (in == NULL) {
        printf("[Util] preprocess(), can not open data file.\n");
        return;
    }
    int t = clock();
    for (int k = 0; k < dataSize; ++k) {
        int val;
        fscanf(in, "%d", &val);
        // fscanf(in, "%f", &data[k].dim[0]);
        for (int i = 0; i < data[k].dimision; ++i) {
            fscanf(in, "%d", &val);
            // fscanf(in, "%f", &data[k].dim[i]);
            data[k].dim[i] = (float)val;
        }
    }
    t = clock()-t;
    printf("[Util] read training data time: %fs.\n", ((float)t)/CLOCKS_PER_SEC);
    fclose(in);

    /* sort the value on the <randomVectorNum> lines
     * build b+ tree index */
    Buffer* buff = new Buffer[dataSize];
    char treeFileName[] = "./data/tree/***.tree";
    int totalIndexingTime = 0, totalProjectionTime = 0;
    for (int i = 0; i < randomVectorNum; ++i) {
        // projection
        t = clock();
        for (int j = 0; j < dataSize; ++j) {
            buff[j].val = data[j] * randomVec[i];
            buff[j].index = j;
        }
        totalProjectionTime += clock()-t;

        treeFileName[12] = '0' + i / 100;
        treeFileName[13] = '0' + (i % 100) / 10;
        treeFileName[14] = '0' + i % 10;
        sort(buff, buff + dataSize);

        // indexing
        t = clock();
        trees[i].init(treeFileName, pageSize);
        trees[i].bulkLoad(buff, dataSize);
        totalIndexingTime += clock()-t;
    }

    printf("[Util] total projecttion time: %fs.\n", ((float)totalProjectionTime)/CLOCKS_PER_SEC);
    printf("[Util] average projecttion time: %fs.\n", ((float)totalProjectionTime/randomVectorNum)/CLOCKS_PER_SEC);
    printf("[Util] total indexing time: %fs.\n", ((float)totalIndexingTime)/CLOCKS_PER_SEC);
    printf("[Util] average indexing time: %fs.\n", ((float)totalIndexingTime/randomVectorNum)/CLOCKS_PER_SEC);

    delete [] buff;
}

void Util::getQueryData(Vector* query) {
    FILE* in = fopen(queryFile, "r");
    if (in == NULL) {
        printf("[Util] getQueryData(), can not open query file.\n");
        exit(0);
    }
    for (int i = 0; i < querySize; ++i) {
        int val;
        fscanf(in, "%d", &val);
        // fscanf(in, "%f", &query[i].dim[0]);
        for (int j = 0; j < query[i].dimision; ++j) {
            // fscanf(in, "%f", &query[i].dim[j]);
            fscanf(in, "%d", &val);
            query[i].dim[j] = (float)val;
        }
    }
}

void Util::project(const Vector& v, float* vals) const {
    // project vector v on 50 lines
    for (int i = 0; i < randomVectorNum; ++i) {
        vals[i] = randomVec[i] * v;
    }
}
