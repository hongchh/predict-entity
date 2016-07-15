#ifndef __BLOCKFILE__
#define __BLOCKFILE__

class BlockFile {
public:
    BlockFile(const char* fileName, const int& pageSize);
    ~BlockFile();
    /* read page at <index> to <page> */
    void readPage(char* page, const int& index);
    /* write <page> into page at <index> */
    void writePage(char* page, const int& index);
    /* append <page> at the end of file */
    void appendPage(char* page);
    /* get the last page in the file, save it in <page> */
    void getLastPage(char* page);
    /* update and write header information into file */
    void updateHeadInfo();
    /* clear the file content */
    void clearFile();
    /* check if the file is new */
    bool isNewFile() const { return _isNewFile; }
    /* get the page size */
    int getPageSize() const { return pageSize; }
    /* get the number of pages */
    int getPageNum() const { return pageNum; }
private:
    FILE* fp;         // a file pointer
    char* fileName;   // the name of the file
    bool _isNewFile;  // check if the file is new
    int pageSize;     // page size (bytes), eg. 1024 bytes per page
    int pageNum;      // number of pages in the file, including the head page
};

#endif