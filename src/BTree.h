#ifndef __BTREE__
#define __BTREE__

class BTree {
public:
    BTree(): bf(NULL), root(NULL) {}
    ~BTree();
    /* initialize a b+ tree */
    void init(const char* fileName, const int& pageSize);
    /* loading data from <buff> and build tree
     * store the tree into disk */
    void bulkLoad(Buffer* buff, const int& buffSize);
    /* read the tree node from disk
     * the page to be read is saved in the <pageId> page
     * store the node in <node> */
    void getNode(Node* node, const int& pageId) const;
    /* given a value <val>, find to pointer <h> and <l>
     * <h> and <l> satisfy that value_h <= <val> <= value_l
     * store the nodes which <h> and <l> belong to at <node_h> and <node_l> */
    void find(const float& val, int* h, int* l, Node* node_h, Node* node_l) const;
private:
    BlockFile* bf;  // manage the tree file
    Node* root;     // save the root node of the tree
};

#endif
