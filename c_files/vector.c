/**
 * @file vector.c
 * @brief Implementation of 3D vector operations.
 */

#include "../header/vector.h"
#include <math.h>
#include <stdio.h>

Vector vector_create(double x, double y, double z)
{
    Vector v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

Vector vector_add(Vector a, Vector b)
{
    Vector result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

Vector vector_sub(Vector a, Vector b)
{
    Vector result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

Vector vector_scale(Vector v, double s)
{
    Vector result;
    result.x = v.x * s;
    result.y = v.y * s;
    result.z = v.z * s;
    return result;
}

double vector_norm(Vector v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

void vector_test(void)
{
    Vector a = vector_create(1.0, 2.0, 3.0);
    Vector b = vector_create(4.0, 5.0, 6.0);

    Vector sum  = vector_add(a, b);
    Vector diff = vector_sub(a, b);
    Vector scaled = vector_scale(a, 2.0);
    double norm = vector_norm(a);

    printf("=== vector_test ===\n");
    printf("a         = (%.2f, %.2f, %.2f)\n", a.x, a.y, a.z);
    printf("b         = (%.2f, %.2f, %.2f)\n", b.x, b.y, b.z);
    printf("a + b     = (%.2f, %.2f, %.2f)  [expected (5,7,9)]\n",
           sum.x, sum.y, sum.z);
    printf("a - b     = (%.2f, %.2f, %.2f)  [expected (-3,-3,-3)]\n",
           diff.x, diff.y, diff.z);
    printf("a * 2     = (%.2f, %.2f, %.2f)  [expected (2,4,6)]\n",
           scaled.x, scaled.y, scaled.z);
    printf("||a||     = %.6f              [expected 3.741657]\n", norm);
    printf("===================\n");
}
