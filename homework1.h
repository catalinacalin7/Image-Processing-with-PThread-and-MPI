#ifndef HOMEWORK_H1
#define HOMEWORK_H1

typedef struct {
  unsigned char **gray_levels;
}image;

int is_black(double x, double y);
void* threadFunction(void *var);

void initialize(image *im);
void render(image *im);
void writeData(const char * fileName, image *img);

#endif /* HOMEWORK_H1 */
