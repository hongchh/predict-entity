#include "headers.h"

void Vector::init(const int& dimision) {
    this->dimision = dimision;
    if (dimision <= 0) {
        dim = NULL;
    } else {
        dim = new float[dimision];
    }
}

Vector::~Vector() {
    if (dim != NULL)
        delete [] dim;
}

float Vector::operator*(const Vector& v) const {
    if (dimision != v.dimision) {
        printf("[Vector] operator*(), dimision error.\n");
        exit(0);
    }
    float val = 0.0;
    for (int i = 0; i < dimision; ++i)
        val += dim[i] * v.dim[i];
    return val;
}

float Vector::distance(const Vector& v) const {
    if (dimision != v.dimision) {
        printf("[Vector] distance(), dimision error.\n");
        exit(0);
    }
    float dist = 0.0;
    for (int i = 0; i < dimision; ++i)
        dist += (dim[i] - v.dim[i]) * (dim[i] - v.dim[i]);
    return sqrt(dist);
}

bool Buffer::operator<(const Buffer& buff) const {
    return val < buff.val;
}
