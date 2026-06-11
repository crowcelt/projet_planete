#ifndef VECTOR_H
#define VECTOR_H

/**
 * @file vector.h
 * @brief 3D vector structure and associated mathematical operations.
 */

/**
 * @struct Vector
 * @brief Represents a 3D vector with double precision components.
 */
typedef struct {
    double x;
    double y;
    double z;
} Vector;

/**
 * @brief Creates a new vector from three components.
 * @param x X component
 * @param y Y component
 * @param z Z component
 * @return Initialized Vector
 */
Vector vector_create(double x, double y, double z);

/**
 * @brief Adds two vectors.
 * @param a First vector
 * @param b Second vector
 * @return a + b
 */
Vector vector_add(Vector a, Vector b);

/**
 * @brief Subtracts vector b from vector a.
 * @param a First vector
 * @param b Second vector
 * @return a - b
 */
Vector vector_sub(Vector a, Vector b);

/**
 * @brief Multiplies a vector by a scalar.
 * @param v The vector
 * @param s The scalar
 * @return v * s
 */
Vector vector_scale(Vector v, double s);

/**
 * @brief Computes the Euclidean norm of a vector.
 * @param v The vector
 * @return ||v||
 */
double vector_norm(Vector v);

/**
 * @brief Runs unit tests for vector operations (prints results to stdout).
 */
void vector_test(void);

#endif /* VECTOR_H */
