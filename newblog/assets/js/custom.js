document.addEventListener("DOMContentLoaded", function () {
  document.querySelectorAll("asciinema-player[src]").forEach(function (tag) {
    var options = {};
    var speed = tag.getAttribute("speed");
    if (speed) options.speed = parseFloat(speed);
    if (tag.getAttribute("loop") !== null) options.loop = true;
    if (tag.getAttribute("autoplay") !== null) options.autoPlay = true;

    var container = document.createElement("div");
    tag.replaceWith(container);
    AsciinemaPlayer.create(tag.getAttribute("src"), container, options);
  });
});
