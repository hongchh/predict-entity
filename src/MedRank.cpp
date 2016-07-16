#include "headers.h"

MedRank::MedRank(const int& objNum, const int& lineNum, const int& pointNum, const float& MINFREQ) {
    this->objNum   = objNum;
    this->lineNum  = lineNum;
    this->pointNum = pointNum;
    this->MINFREQ  = MINFREQ;

    l    = new int[lineNum];
    h    = new int[lineNum];
    fc   = new int[objNum];
    node = new Node[lineNum * 2];
}

MedRank::~MedRank() {
    if (l != NULL)
        delete [] l;
    if (h != NULL)
        delete [] h;
    if (fc != NULL)
        delete [] fc;
    if (node != NULL)
        delete [] node;
}

void MedRank::medrank(const BTree* trees, const float* queryProjectVal, int& probNum, int* index) {
    // find_two_pointers
    for (int i = 0; i < lineNum; ++i) {
        trees[i].find(queryProjectVal[i], h+i, l+i, node+i*2, node+i*2+1);
    }
    // medrank
    memset(fc, 0, sizeof(int)*(objNum));
    int curIndex;
    map<int, int> mark;
    while (mark.size() < pointNum) {
        for (int i = 0; i < lineNum; ++i) {
            /* compare ith lines |vi,hi - qi| and |vi,li - qi|
             * if ith lines l or h out of range, set as -1 */
            if (l[i] != -1 && h[i] != -1) {
                // |vi,hi - qi| > |vi,li - qi|
                if (fabs(node[i*2].entry[h[i]] - queryProjectVal[i]) < \
                    fabs(node[i*2+1].entry[l[i]] - queryProjectVal[i])) {
                    curIndex = node[i*2].sonOrIndex[h[i]];
                    --h[i];
                    // h[i]<0 and node[i*2].left exit, update the data of node, or h[i] = -1
                    if (h[i] < 0 && node[i*2].left != 0) {
                        trees[i].getNode(node+i*2, node[i*2].left);
                        ++probNum;
                        h[i] += NODE_ENTRY_SIZE;
                    } else if (h[i] < 0) {
                        h[i] = -1;
                    }
                } else {
                    curIndex = node[i*2+1].sonOrIndex[l[i]];
                    ++l[i];
                    // l[i]<0 and node[i*2+1].right exit, update the data of node, or l[i] = -1
                    if (l[i] >= node[i*2+1].entryNum && node[i*2+1].right != 0) {
                        trees[i].getNode(node+i*2+1, node[i*2+1].right);
                        ++probNum;
                        l[i] = 0;
                    } else if (l[i] >= node[i*2+1].entryNum) {
                        l[i] = -1;
                    }
                }
            }
            // h out of range, l not out of range
            else if (l[i] != -1) {
                curIndex = node[i*2+1].sonOrIndex[l[i]];
                ++l[i];
                // l[i]<0 and node[i*2+1].right exit, update the data of node, or l[i] = -1
                if (l[i] >= node[i*2+1].entryNum && node[i*2+1].right != 0) {
                    trees[i].getNode(node+2*i+1, node[i*2+1].right);
                    ++probNum;
                    l[i] = 0;
                } else if (l[i] >= node[i*2+1].entryNum) {
                    l[i] = -1;
                }
            }
            // l out of range, h not out of range
            else if (h[i] != -1) {
                curIndex = node[i*2].sonOrIndex[h[i]];
                --h[i];
                // h[i]<0 and node[i*2].left exit, update the data of node, or h[i] = -1
                if (h[i] < 0 && node[i*2].left != 0) {
                    trees[i].getNode(node+i*2, node[i*2].left);
                    ++probNum;
                    h[i] += NODE_ENTRY_SIZE;
                } else if (h[i] < 0) {
                    h[i] = -1;
                }
            }
            // h, l out of range
            else {
                continue;
            }
            
            ++fc[curIndex];
            if (fc[curIndex] > (int)(lineNum * MINFREQ)) {
                mark[curIndex] = 1;
            }
        }
    }
    // save index
    map<int, int>::iterator it = mark.begin();
    for (int i = 0; it != mark.end(); ++it, ++i) {
        index[i] = it->first;
    }
}

void MedRank::findClosestPoint(const Vector* data, const Vector& q, int* index) {
    Buffer buff = new Buffer[objNum];
    for (int i = 0; i < objNum; ++i) {
        buff[i].val   = data[i].distance(q);
        buff[i].index = i;
    }
    sort(buff, buff + objNum);
    for (int i = 0; i < pointNum; ++i) {
        index[i] = buff[objNum - pointNum + i].index;
    }
}
