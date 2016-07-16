#include "headers.h"

BTree::~BTree() {
    if (bf != NULL)
        delete bf;
    if (root != NULL)
        delete root;
}

void BTree::init(const char* fileName, const int& pageSize) {
    bf = new BlockFile(fileName, pageSize);
    if (bf->isNewFile()) {
        root = NULL;
    } else {
        char page[bf->getPageSize()];
        bf->getLastPage(page);
        root = new Node();
        memcpy((void*)root, (void*)page, sizeof(Node));
    }
}

void BTree::bulkLoad(Buffer* buff, const int& buffSize) {
    // if the file already contains something, clear it
    if (!bf->isNewFile()) {
        bf->clearFile();
    }
    char page[bf->getPageSize()];
    memset((void*)page, 0, sizeof(page));
    int currentLevelPageNum = 0;
    int pageIndex = 1;
    int level = 0;
    Node* node = new Node(0);
    // build level 0
    for (int i = 0; i < buffSize; ++currentLevelPageNum, ++pageIndex) {
        node->entryNum = 0;
        for (int j = 0; j < NODE_ENTRY_SIZE && i < buffSize; ++i, ++j) {
            node->entry[j] = buff[i].val;
            node->sonOrIndex[j] = buff[i].index;
            ++node->entryNum;
        }
        // on level 0, currentLevelPageNum can indicate the previous node's pageId
        node->left = currentLevelPageNum;
        // if last node, right = 0
        node->right = (i == buffSize ? 0 : currentLevelPageNum+2);
        node->pageId = pageIndex;
        // convert Node to page and write into file
        memcpy((void*)page, (void*)node, sizeof(Node));
        bf->appendPage(page);
    }

    // build b+ tree level by level
    int currentLevelBeginPid;
    int lastLevelBeginPid = 1;
    int lastLevelPageNum;
    // while currentLevelPageNum == 1, it reaches root node, loop stop
    while (currentLevelPageNum > 1) {
        ++level;
        lastLevelPageNum = currentLevelPageNum;
        currentLevelPageNum = 0;
        currentLevelBeginPid = pageIndex;
        node->level = level;
        for (int i = 0; i < lastLevelPageNum; ++currentLevelPageNum, ++pageIndex) {
            Node tmp(0);
            node->entryNum = 0;
            for (int j = 0; j < NODE_ENTRY_SIZE && i < lastLevelPageNum; ++i, ++j) {
                bf->readPage(page, lastLevelBeginPid+i);
                memcpy((void*)&tmp, (void*)page, sizeof(tmp));
                node->entry[j] = tmp.entry[0];
                node->sonOrIndex[j] = tmp.pageId;
                ++node->entryNum;
            }
            // if first node, left = 0; else, pageIndex-1 can indicate the previous node
            node->left = (currentLevelPageNum == 0 ? 0 : pageIndex-1);
            // if last node, right = 0; else, pageInde+1 can indicate the next node
            node->right = (i == lastLevelPageNum ? 0 : pageIndex+1);
            node->pageId = pageIndex;
            // convert Node to page and write it to file
            memcpy((void*)page, (void*)node, sizeof(Node));
            bf->appendPage(page);
        }
        lastLevelBeginPid = currentLevelBeginPid;
    }
    // update headInfo in the file
    bf->updateHeadInfo();
    // save the root node
    root = node;
}

void BTree::getNode(Node* node, const int& pageId) const {
    if (pageId <= 0 || pageId >= bf->getPageNum()) {
        printf("[BTree] getNode(), pageId error.\n");
        exit(0);
    }
    char page[bf->getPageSize()];
    bf->readPage(page, pageId);
    memcpy((void*)node, (void*)page, sizeof(Node));
}

void BTree::find(const float& val, int* h, int* l, Node* node_h, Node* node_l) const {
    char page[bf->getPageSize()];
    Node tmp;
    /* if val > entry[entryNum-1], pageId should be the last one
     * so, just initialize pageId to the last one
     * if val <= entry[0], it doesn't mater, pageId will be the first one */
    int pageId = root->sonOrIndex[root->entryNum-1];
    for (int i = 1; i < root->entryNum; ++i) {
        if (val <= root->entry[i]) {
            pageId = root->sonOrIndex[i-1];
            break;
        }
    }
    // find leaf page
    bf->readPage(page, pageId);
    memcpy((void*)&tmp, (void*)page, sizeof(tmp));
    while (tmp.level != 0) {
        pageId = tmp.sonOrIndex[tmp.entryNum-1];
        for (int i = 1; i < tmp.entryNum; ++i) {
            if (val <= tmp.entry[i]) {
                pageId = tmp.sonOrIndex[i-1];
                break;
            }
        }
        bf->readPage(page, pageId);
        memcpy((void*)&tmp, (void*)page, sizeof(tmp));
    }
    // find h and l
    for (int i = 0; i < tmp.entryNum; ++i) {
        if (val <= tmp.entry[i]) {
            *l = i;
            *h = i-1;
            break;
        }
        // val > last entry
        if (i == tmp.entryNum-1) {
            *h = i;
            *l = i+1;
        }
    }

    // copy entry[h]'s Node
    if (*h >= 0) {
        memcpy((void*)node_h, (void*)&tmp, sizeof(tmp));
    }
    // if *h < 0, the entry (which < val) in the previous page (if there is previous page)
    else if (tmp.left != 0) {
        *h += NODE_ENTRY_SIZE;
        bf->readPage(page, tmp.left);
        memcpy((void*)node_h, (void*)page, sizeof(tmp));
    }
    /* if there is no previous page, ie. tmp is the first page
     * set nl->pageId to 0, it means that NULL, no entry < val */
    else {
        *h = -1;
        node_h->pageId = 0;
    }

    // copy entry[l]'s Node
    if (*l < NODE_ENTRY_SIZE) {
        memcpy((void*)node_l, (void*)&tmp, sizeof(tmp));
    }
    // if *l >= NODE_ENTRY_SIZE, the entry (which > val) in the next page (if there is next page)
    else if (tmp.right != 0) {
        *l -= NODE_ENTRY_SIZE;
        bf->readPage(page, tmp.right);
        memcpy((void*)node_l, (void*)page, sizeof(tmp));
    }
    /* if there is no next page, ie. tmp is the last page
     * set nl->pageId to 0, it means that NULL, no entry > val */
    else {
        *l = -1;
        node_l->pageId = 0;
    }
}
