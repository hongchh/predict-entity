#include <cstdio>
using namespace std;

int main() {
  unsigned char obj[784], label;
  // get query data and label
  FILE* in1 = fopen("t10k-images.idx3-ubyte", "rb");
  FILE* in2 = fopen("t10k-labels.idx1-ubyte", "rb");
  FILE* out1 = fopen("Mnist-q.txt", "w");
  fread((void*)obj, 4, 4, in1);
  fread((void*)obj, 2, 4, in2);
  for (int i = 0; i < 300; ++i) {
    fread((void*)obj, 784, 1, in1);
    fread((void*)&label, 1, 1, in2);
    fprintf(out1, "%d", label);
    for (int j = 0; j < 784; ++j) {
      fprintf(out1, " %d", obj[j]);
    }
    fprintf(out1, "\n");
  }
  fclose(in1);
  fclose(in2);
  fclose(out1);

  // get training data and label
  FILE* in3 = fopen("train-images.idx3-ubyte", "rb");
  FILE* in4 = fopen("train-labels.idx1-ubyte", "rb");
  FILE* out2 = fopen("Mnist-d.txt", "w");
  fread((void*)obj, 4, 4, in3);
  fread((void*)obj, 2, 4, in4);
  for (int i = 0; i < 60000; ++i) {
    fread((void*)obj, 784, 1, in3);
    fread((void*)&label, 1, 1, in4);
    fprintf(out2, "%d", label);
    for (int j = 0; j < 784; ++j) {
      fprintf(out2, " %d", obj[j]);
    }
    fprintf(out2, "\n");
  }
  fclose(in3);
  fclose(in4);
  fclose(out2);
  return 0;
}
