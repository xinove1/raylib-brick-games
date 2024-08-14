#ifndef XI_TYPES_H_
# define XI_TYPES_H_

# include "raylib.h"
# include "stdint.h"
# include "stddef.h"

// Utilities
# define MAX(a, b) ((a)>(b)? (a) : (b))
# define MIN(a, b) ((a)<(b)? (a) : (b))

// From  https://github.com/EpicGamesExt/raddebugger/blob/77edf0d8dad8175992431e466bd0e59d8bddad5d/src/base/base_core.h#L65
# define KB(n)  (((u64)(n)) << 10)
# define MB(n)  (((u64)(n)) << 20)
# define GB(n)  (((u64)(n)) << 30)
# define TB(n)  (((u64)(n)) << 40)
# define Thousand(n)   ((n)*1000)
# define Million(n)    ((n)*1000000)
# define Billion(n)    ((n)*1000000000)

// Alias for raylib types
typedef Vector2 V2;
typedef Rectangle Rect;

// Types
typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef char cstr;
typedef i32  b32;
typedef char byte; // TODO  Update to unsigned char? or u8

typedef ptrdiff_t size;
typedef size_t    usize;

#endif // XI_TYPES_H_
