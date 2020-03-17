#ifndef HOMEWORK_H
#define HOMEWORK_H


// pentru a reprezenta un pixel color
typedef struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} rgb;

// structura imaginii
typedef struct {
  char format[2];
  int width;
  int height;
  int maxval;
  // pentru stocarea pixelilor imaginii color
  rgb **rgb_levels;
  // pentru stocarea pixelilor imaginii bw
  unsigned char **gray_levels;
}image;

int get_media_even(int x, int y);
rgb get_media_rgb_even( int x, int y);
int get_media_odd(int x, int y);
rgb get_media_rgb_odd( int x, int y);
void* threadFunction(void *var);

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */
