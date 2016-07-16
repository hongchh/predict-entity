#ifndef __MEDRANK__
#define __MEDRANK__

class MedRank {
public:
    MedRank(const int& objNum, const int& lineNum, const int& pointNum, const float& MINFREQ);
    ~MedRank();
    /* use the index <trees> and the project value of a query point <queryProjectVal>
     * to find top N points which appromixate closest to a query point
     * save the point index in <index> */
    void medrank(const BTree* trees, const float* queryProjectVal, int& probNum, int* index);
    /* find top N points in <data> which are closest to <q>
     * save the point index in <index> */
    void findClosestPoint(const Vector* data, const Vector& q, int* index);
private:
    int objNum;        // number of objects in the training data
    int lineNum;       // number of the projection lines
    int pointNum;      // number of the top N closest points
    int *l, *h, *fc;   // l and h are pointers, fc are frequency count
    float MINFREQ;     // the parameter MINFREQ of medrank algorithm
    Node* node;        // save the nodes which pointers <l> and <h> point to
};

#endif