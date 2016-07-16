#ifndef __DATASTRUCTURE__
#define __DATASTRUCTURE__

struct Vector {
    Vector() {}
    ~Vector();
    void init(const int& dimision = 0);
    float operator*(const Vector& v) const;
    float distance(const Vector& v) const;
    int dimision;  // this dimision of a vector
    float* dim;    // the value in each dimision of a vector
};

struct Buffer {
    float val;  // the projection value
    int index;  // the index of the projected point
    bool operator<(const Buffer& buff) const;
};

struct Node {
    Node(const char& level = 0): level(level), entryNum(0), left(0), right(0) {}
    char level;                       // the level in the b+ tree
    int pageId;                       // the page id of this node
    int left, right;                  // the left and right brother of this node
    int entryNum;                     // the number of entry save in this node
    float entry[NODE_ENTRY_SIZE];     // entry of a node
    int sonOrIndex[NODE_ENTRY_SIZE];  // the son of a index node, or the index of a leaf node
};

#endif
