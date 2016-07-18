#include <cstdio>
#include <cstring>
#include <algorithm>
using namespace std;

int main() {
  FILE* in1 = fopen("../data/detail.txt", "r");
  FILE* in2 = fopen("../data/color-q.txt", "r");
  FILE* out = fopen("./color.txt", "w");
  fprintf(out, "var data = [");
  int type, r, g, b;
  for (int i = 0; i < 100; ++i) {
    fscanf(in1, "[Query %3d] %d (approximate), %d (real)\n", &r, &type, &g);
    fscanf(in2, "%d %d %d", &r, &g, &b);
    fprintf(out, "{type:%d, color:\"rgb(%d, %d, %d)\"},", type, r, g, b);
  }
  fprintf(out, "];\n");
  fclose(in1);
  fclose(in2);
  fclose(out);
  return 0;
}
