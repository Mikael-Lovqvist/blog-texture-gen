// Adapted from https://gist.github.com/ciembor/1494530


pixel_format crgb(int r, int g, int b) {
  return (pixel_format) {
    r > 255 ? 255 : (r < 0 ? 0 : r),
    g > 255 ? 255 : (g < 0 ? 0 : g),
    b > 255 ? 255 : (b < 0 ? 0 : b),
  };
}

pixel_format frgb(float r, float g, float b) {
  return crgb(r * 255.0, g * 255.0, b * 255.0);
}

pixel_format fgray(float v) {
  return frgb(v, v, v);
}


/*
 * Converts an HUE to r, g or b.
 * returns float in the set [0, 1].
 */
float huef_to_rgbf(float p, float q, float t) {

  if (t < 0)
    t += 1;
  if (t > 1)
    t -= 1;
  if (t < 1./6)
    return p + (q - p) * 6 * t;
  if (t < 1./2)
    return q;
  if (t < 2./3)
    return p + (q - p) * (2./3 - t) * 6;

  return p;

}

////////////////////////////////////////////////////////////////////////

/*
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns RGB in the set [0, 255].
 */
pixel_format hslf_to_rgb(float h, float s, float l) {

  pixel_format result;

  if(0 == s) {
    result.r = result.g = result.b = l; // achromatic
  }
  else {
    float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    float p = 2 * l - q;
    result.r = huef_to_rgbf(p, q, h + 1./3) * 255;
    result.g = huef_to_rgbf(p, q, h) * 255;
    result.b = huef_to_rgbf(p, q, h - 1./3) * 255;
  }

  return result;

}