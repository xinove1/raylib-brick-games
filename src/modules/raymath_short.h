/**********************************************************************************************
*   Modified Version of raymath v1.5, shorting funcs names, changing types and removing unusead funcs
*      raymath v1.5 - Math functions to work with Vector2, Vector3, Matrix and Quaternions
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2015-2023 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/
#ifndef RAYMATH_SHORT_H_
# define RAYMATH_SHORT_H_
# include "types.h"
# include <math.h>

#ifdef RAYMATH_SHORT_IMPLEMENTATION
# define RMSAPI extern inline
#else
# define RMSAPI inline
#endif

// Creator helpers

// Crate Rectangle from to Vectors
RMSAPI Rect RectV2(V2 pos, V2 size) {
	return ((Rect) {.x = pos.x, .y = pos.y, .width = size.x, .height = size.y});
}

RMSAPI V2 V2RectPos(Rect rect) {
	return ((V2) {rect.x, rect.y});
}

RMSAPI V2 V2RectSize(Rect rect) {
	return ((V2) {rect.width, rect.height});
}

RMSAPI V2 V2F32(f32 value) {
	return ((V2) {value, value});
}


//----------------------------------------------------------------------------------
// Module Functions Definition - Utils math
//----------------------------------------------------------------------------------
//
// Prepend f32 to names to not collide with raylibs use of raymath, (seems to only matter in static builds)

// Clamp f32 value
RMSAPI f32 f32Clamp(f32 value, f32 min, f32 max)
{
    f32 result = (value < min)? min : value;

    if (result > max) result = max;

    return result;
}

// Calculate linear interpolation between two f32s
RMSAPI f32 f32Lerp(f32 start, f32 end, f32 amount)
{
    f32 result = start + amount*(end - start);

    return result;
}

// Normalize input value within input range
RMSAPI f32 f32Normalize(f32 value, f32 start, f32 end)
{
    f32 result = (value - start)/(end - start);

    return result;
}

// Remap input value within input range to output range
RMSAPI f32 f32Remap(f32 value, f32 inputStart, f32 inputEnd, f32 outputStart, f32 outputEnd)
{
    f32 result = (value - inputStart)/(inputEnd - inputStart)*(outputEnd - outputStart) + outputStart;

    return result;
}

// Wrap input value from min to max
RMSAPI f32 f32Wrap(f32 value, f32 min, f32 max)
{
    f32 result = value - (max - min)*floorf((value - min)/(max - min));

    return result;
}

// Check whether two given f32s are almost equal
RMSAPI i32 f32Equals(f32 x, f32 y)
{
#if !defined(EPSILON)
    #define EPSILON 0.000001f
#endif

    i32 result = (fabsf(x - y)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))));

    return result;
}

//----------------------------------------------------------------------------------
// Module Functions Definition - V2 math
//----------------------------------------------------------------------------------
RMSAPI b32 V2Compare(V2 v1, V2 v2) 
{
    return (v1.x == v2.x && v1.y == v2.y);
}

// Vector with components value 0.0f
RMSAPI V2 V2Zero(void)
{
    V2 result = { 0.0f, 0.0f };

    return result;
}

// Vector with components value 1.0f
RMSAPI V2 V2One(void)
{
    V2 result = { 1.0f, 1.0f };

    return result;
}

// Add two vectors (v1 + v2)
RMSAPI V2 V2Add(V2 v1, V2 v2)
{
    V2 result = { v1.x + v2.x, v1.y + v2.y };

    return result;
}

// Add vector and f32 value
RMSAPI V2 V2AddValue(V2 v, f32 add)
{
    V2 result = { v.x + add, v.y + add };

    return result;
}

// Subtract two vectors (v1 - v2)
RMSAPI V2 V2Subtract(V2 v1, V2 v2)
{
    V2 result = { v1.x - v2.x, v1.y - v2.y };

    return result;
}

// Subtract vector by f32 value
RMSAPI V2 V2SubtractValue(V2 v, f32 sub)
{
    V2 result = { v.x - sub, v.y - sub };

    return result;
}

// Calculate vector length
RMSAPI f32 V2Length(V2 v)
{
    f32 result = sqrtf((v.x*v.x) + (v.y*v.y));

    return result;
}

// Calculate vector square length
RMSAPI f32 V2LengthSqr(V2 v)
{
    f32 result = (v.x*v.x) + (v.y*v.y);

    return result;
}

// Calculate two vectors dot product
RMSAPI f32 V2DotProduct(V2 v1, V2 v2)
{
    f32 result = (v1.x*v2.x + v1.y*v2.y);

    return result;
}

// Calculate distance between two vectors
RMSAPI f32 V2Distance(V2 v1, V2 v2)
{
    f32 result = sqrtf((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));

    return result;
}

// Calculate square distance between two vectors
RMSAPI f32 V2DistanceSqr(V2 v1, V2 v2)
{
    f32 result = ((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));

    return result;
}

// Calculate angle between two vectors
// NOTE: Angle is calculated from origin point (0, 0)
RMSAPI f32 V2Angle(V2 v1, V2 v2)
{
    f32 result = 0.0f;

    f32 dot = v1.x*v2.x + v1.y*v2.y;
    f32 det = v1.x*v2.y - v1.y*v2.x;

    result = atan2f(det, dot);

    return result;
}

// Calculate angle defined by a two vectors line
// NOTE: Parameters need to be normalized
// Current implementation should be aligned with glm::angle
RMSAPI f32 V2LineAngle(V2 start, V2 end)
{
    f32 result = 0.0f;

    // TODO(10/9/2023): Currently angles move clockwise, determine if this is wanted behavior
    result = -atan2f(end.y - start.y, end.x - start.x);

    return result;
}

// Scale vector (multiply by value)
RMSAPI V2 V2Scale(V2 v, f32 scale)
{
    V2 result = { v.x*scale, v.y*scale };

    return result;
}

// Multiply vector by vector
RMSAPI V2 V2Multiply(V2 v1, V2 v2)
{
    V2 result = { v1.x*v2.x, v1.y*v2.y };

    return result;
}

// Negate vector
RMSAPI V2 V2Negate(V2 v)
{
    V2 result = { -v.x, -v.y };

    return result;
}

// Divide vector by vector
RMSAPI V2 V2Divide(V2 v1, V2 v2)
{
    V2 result = { v1.x/v2.x, v1.y/v2.y };

    return result;
}

// Normalize provided vector
RMSAPI V2 V2Normalize(V2 v)
{
    V2 result = { 0 };
    f32 length = sqrtf((v.x*v.x) + (v.y*v.y));

    if (length > 0)
    {
        f32 ilength = 1.0f/length;
        result.x = v.x*ilength;
        result.y = v.y*ilength;
    }

    return result;
}

// Transforms a V2 by a given Matrix
RMSAPI V2 V2Transform(V2 v, Matrix mat)
{
    V2 result = { 0 };

    f32 x = v.x;
    f32 y = v.y;
    f32 z = 0;

    result.x = mat.m0*x + mat.m4*y + mat.m8*z + mat.m12;
    result.y = mat.m1*x + mat.m5*y + mat.m9*z + mat.m13;

    return result;
}

// Calculate linear interpolation between two vectors
RMSAPI V2 V2Lerp(V2 v1, V2 v2, f32 amount)
{
    V2 result = { 0 };

    result.x = v1.x + amount*(v2.x - v1.x);
    result.y = v1.y + amount*(v2.y - v1.y);

    return result;
}

// Calculate reflected vector to normal
RMSAPI V2 V2Reflect(V2 v, V2 normal)
{
    V2 result = { 0 };

    f32 dotProduct = (v.x*normal.x + v.y*normal.y); // Dot product

    result.x = v.x - (2.0f*normal.x)*dotProduct;
    result.y = v.y - (2.0f*normal.y)*dotProduct;

    return result;
}

// Rotate vector by angle
RMSAPI V2 V2Rotate(V2 v, f32 angle)
{
    V2 result = { 0 };

    f32 cosres = cosf(angle);
    f32 sinres = sinf(angle);

    result.x = v.x*cosres - v.y*sinres;
    result.y = v.x*sinres + v.y*cosres;

    return result;
}

// Move Vector towards target
RMSAPI V2 V2MoveTowards(V2 v, V2 target, f32 maxDistance)
{
    V2 result = { 0 };

    f32 dx = target.x - v.x;
    f32 dy = target.y - v.y;
    f32 value = (dx*dx) + (dy*dy);

    if ((value == 0) || ((maxDistance >= 0) && (value <= maxDistance*maxDistance))) return target;

    f32 dist = sqrtf(value);

    result.x = v.x + dx/dist*maxDistance;
    result.y = v.y + dy/dist*maxDistance;

    return result;
}

// Invert the given vector
RMSAPI V2 V2Invert(V2 v)
{
    V2 result = { 1.0f/v.x, 1.0f/v.y };

    return result;
}

// Clamp the components of the vector between
// min and max values specified by the given vectors
RMSAPI V2 V2Clamp(V2 v, V2 min, V2 max)
{
    V2 result = { 0 };

    result.x = fminf(max.x, fmaxf(min.x, v.x));
    result.y = fminf(max.y, fmaxf(min.y, v.y));

    return result;
}

// Clamp the magnitude of the vector between two min and max values
RMSAPI V2 V2ClampValue(V2 v, f32 min, f32 max)
{
    V2 result = v;

    f32 length = (v.x*v.x) + (v.y*v.y);
    if (length > 0.0f)
    {
        length = sqrtf(length);

        if (length < min)
        {
            f32 scale = min/length;
            result.x = v.x*scale;
            result.y = v.y*scale;
        }
        else if (length > max)
        {
            f32 scale = max/length;
            result.x = v.x*scale;
            result.y = v.y*scale;
        }
    }

    return result;
}

// Check whether two given vectors are almost equal
RMSAPI i32 V2Equals(V2 p, V2 q)
{
#if !defined(EPSILON)
    #define EPSILON 0.000001f
#endif

    i32 result = ((fabsf(p.x - q.x)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(p.x), fabsf(q.x))))) &&
                  ((fabsf(p.y - q.y)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(p.y), fabsf(q.y)))));

    return result;
}

#endif
