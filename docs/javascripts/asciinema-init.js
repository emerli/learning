document.querySelectorAll("asciinema-player[src]").forEach((tag) => {
  const options = {};
  const speed = tag.getAttribute("speed");
  if (speed) options.speed = parseFloat(speed);
  const loop = tag.getAttribute("loop");
  if (loop !== null) options.loop = true;
  const autoplay = tag.getAttribute("autoplay");
  if (autoplay !== null) options.autoPlay = true;

  const container = document.createElement("div");
  tag.replaceWith(container);
  AsciinemaPlayer.create(tag.getAttribute("src"), container, options);
});
