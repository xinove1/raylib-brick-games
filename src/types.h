#ifndef XI_TYPES_H_
# define XI_TYPES_H_

// Utilities
# define MAX(a, b) ((a)>(b)? (a) : (b))
# define MIN(a, b) ((a)<(b)? (a) : (b))

// Foward declaring types from raylib
typedef struct Vector2 Vector2;
typedef struct Rectangle Rectangle;

// Alias for raylib types
typedef Vector2 V2;
typedef Rectangle Rect;

#endif
