#ifndef TYPE_UI

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	int width;
	int height;
} Size;

typedef struct {
	Point point;
	Size size;	
} Definition;

#define TYPE_UI

#endif