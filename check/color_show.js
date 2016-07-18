(function() {
  'use strict';

  // paste data here

  var container = document.getElementById("container");
  for (var i = 0; i < 100; ++i) {
    var div = document.createElement("div");
    div.className = "grid";
    div.id = i;
    div.innerText = data[i].type;
    div.style.backgroundColor = data[i].color;
    container.appendChild(div);
  }
})();
