#include <cstdio>
using namespace std;

int main() {
  FILE* in1 = fopen("../data/detail.txt", "r");
  FILE* in2 = fopen("../data/color-q.txt", "r");
  FILE* out = fopen("./color_show.js", "w");
  fprintf(out, "window.onload = function() { var data = [");
  int type_m, type_r, r, g, b;
  for (int i = 0; i < 100; ++i) {
    fscanf(in1, "[Query %3d] %d (medrank), %d (bf), %d (real).\n", &r, &type_m, &g, &b);
    fscanf(in2, "%d %d %d %d", &type_r, &r, &g, &b);
    fprintf(out, "{type:%d, color:\"rgb(%d, %d, %d)\"},", type_m, r, g, b);
  }
  fprintf(out, "];var container = document.getElementById(\"container\");for (var i = 0; i < 100; ++i) {var div = document.createElement(\"div\");div.className = \"grid\";div.id = i;div.innerText = data[i].type;div.style.backgroundColor = data[i].color;container.appendChild(div);}};");
  fclose(in1);
  fclose(in2);
  fclose(out);
  return 0;
}
