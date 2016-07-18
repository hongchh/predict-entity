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
    for (int k = 0; k < dataSize; ++k) {
        int val;
        fscanf(in, "%d", &data[k].type);
        // fscanf(in, "%f", &data[k].type);
        for (int i = 0; i < data[k].dimision; ++i) {
            // fscanf(in, "%f", &data[k].dim[i]);
            fscanf(in, "%d", &val);
            data[k].dim[i] = (float)val;
        }
    }
    fclose(in);

    /* sort the value on the <randomVectorNum> lines
     * build b+ tree index */
    Buffer* buff = new Buffer[dataSize];
    char treeFileName[] = "./data/tree/***.tree";
    for (int i = 0; i < randomVectorNum; ++i) {
        // projection
        for (int j = 0; j < dataSize; ++j) {
            buff[j].val = data[j] * randomVec[i];
            buff[j].index = j;
        }

        treeFileName[12] = '0' + i / 100;
        treeFileName[13] = '0' + (i % 100) / 10;
        treeFileName[14] = '0' + i % 10;
        sort(buff, buff + dataSize);

        // indexing
        trees[i].init(treeFileName, pageSize);
        trees[i].bulkLoad(buff, dataSize);
    }

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
        for (int j = 0; j < query[i].dimision; ++j) {
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

int Util::predict(const Vector* data, const int* index, const int& indexSize) const {
    // counting
    map<int, int> mark;
    for (int i = 0; i < indexSize; ++i) {
        if (mark.find(data[index[i]].type) != mark.end()) {
            ++mark[data[index[i]].type];
        } else {
            mark[data[index[i]].type] = 1;
        }
    }
    // find max
    map<int, int>::iterator it = mark.begin();
    int maxCount = 0, type;
    for (; it != mark.end(); ++it) {
        if (maxCount < it->second) {
            maxCount = it->second;
            type = it->first;
        }
    }
    return type;
}
