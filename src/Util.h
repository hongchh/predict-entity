#ifndef __UTIL__
#define __UTIL__

class Util {
public:
    Util(const char* dataFile, const char* queryFile,
         const int& dataSize, const int& randomVectorNum,
         const int& querySize, const int& dimision);
    ~Util();
    /* get training data from dataFile, projection, sorting, build b+ tree index
     * save the training data in <data> */
    void preprocess(BTree* trees, const int& pageSize, Vector* data);
    /* get query data from queryFile, save the query data in <q> */
    void getQueryData(Vector* q);
    /* project a vector <v> on <randomVectorNum> lines
     * save the project value in <vals> */
    void project(const Vector& v, float* vals) const;
private:
    /* generate a random value that satisfy normal distribution */
    float normalDistribution();

    char* dataFile;        // the file name of the training data file
    char* queryFile;       // the file name of the query data file
    int dataSize;          // the number of the object in the training data file
    int querySize;         // the number of the object in the query data file
    int randomVectorNum;   // the number of the random unit projection vectors
    Vector* randomVec;     // the random unit projection vectors
};

#endif
