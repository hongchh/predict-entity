#include "headers.h"

BlockFile::BlockFile(const char* _fileName, const int& _pageSize) {
    fileName = new char[strlen(_fileName)+1];
    strcpy(fileName, _fileName);

    fp = fopen(fileName, "rb+");
    /* if the file does not exist, create a new file and set headInfo
     * else, read headInfo from the existed file */
    if (fp == NULL) {
        if (_pageSize < MIN_PAGE_SIZE) {
            printf("[BlockFile] BlockFile(), page size is too small.\n");
            exit(0);
        }
        fp = fopen(fileName, "wb+");
        if (fp == NULL) {
            printf("[BlockFile] BlockFile(), could not open file.\n");
            exit(0);
        }
        pageSize = _pageSize;
        pageNum = 1;
        _isNewFile = true;
        // write headInfo into new file
        updateHeadInfo();
    } else {
        // read headInfo from file
        fread((void*)(&pageSize), 4, CHAR_SIZE, fp);
        fread((void*)(&pageNum), 4, CHAR_SIZE, fp);
        _isNewFile = false;
    }
}

BlockFile::~BlockFile() {
    if (fileName != NULL)
        delete [] fileName;
    if (fp != NULL)
        fclose(fp);
}

void BlockFile::updateHeadInfo() {
    char* page = new char[pageSize];
    memset((void*)page, 0, pageSize * CHAR_SIZE);
    *((int*)(&page[0])) = pageSize;
    *((int*)(&page[4])) = pageNum;
    fseek(fp, 0, SEEK_SET);
    fwrite((void*)page, pageSize, CHAR_SIZE, fp);
    delete [] page;
}

void BlockFile::readPage(char* page, const int& index) {
    // can not read the page which does not exist
    if (index >= pageNum) {
        printf("[BlockFile] getPage(), index error\n");
        exit(0);
    }
    fseek(fp, index*pageSize, SEEK_SET);
    fread((void*)page, pageSize, CHAR_SIZE, fp);
}

void BlockFile::writePage(char* page, const int& index) {
    /* can not write the page which does not exist
     * can not write the head page */
    if (index >= pageNum || index == 0) {
        printf("[BlockFile] writePage(), index error\n");
        exit(0);
    }
    fseek(fp, index*pageSize, SEEK_SET);
    fwrite((void*)page, pageSize, CHAR_SIZE, fp);
}

void BlockFile::appendPage(char* page) {
    fseek(fp, 0, SEEK_END);
    fwrite((void*)page, pageSize, CHAR_SIZE, fp);
    ++pageNum;
}

void BlockFile::getLastPage(char* page) {
    fseek(fp, -pageSize, SEEK_END);
    fread((void*)page, pageSize, CHAR_SIZE, fp);
}

void BlockFile::clearFile() {
    fclose(fp);
    fp = fopen(fileName, "wb+");
    if (fp == NULL) {
        printf("[BlockFile] clearFile(), could not open file.\n");
        exit(0);
    }
    pageNum = 1;
    _isNewFile = true;
    updateHeadInfo();
}
