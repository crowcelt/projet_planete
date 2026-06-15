#ifndef TRAJECTORY_H
#define TRAJECTORY_H

/**
 * @file trajectory.h
 * @brief Point and Trajectory structures for storing celestial body states.
 *
 * A Point groups a position vector r, a velocity vector v, and a timestamp t.
 * A Trajectory is a dynamic array of Points representing a body's path over time.
 */

#include "vector.h"

/** Initial capacity of the points array. */
#define TRAJECTORY_INIT_CAPACITY 1024

/**
 * @struct Point
 * @brief A single state on a trajectory: position, velocity and time.
 */
typedef struct {
    Vector r; /**< Position vector (m) */
    Vector v; /**< Velocity vector (m/s) */
    double t; /**< Real time in seconds */
} Point;

/**
 * @brief Creates a new point.
 * @param r Position vector
 * @param v Velocity vector
 * @param t Time step index
 * @return Initialized Point
 */
Point point_create(Vector r, Vector v, double t);

/**
 * @brief Runs unit tests for point creation (prints results to stdout).
 */
void point_test(void);

/**
 * @struct Trajectory
 * @brief Stores a sequence of points representing a body's path over time.
 */
typedef struct {
    Point *points;   /**< Dynamic array of points */
    int    size;     /**< Current number of points */
    int    capacity; /**< Allocated capacity */
} Trajectory;

/**
 * @brief Initializes a trajectory (allocates memory).
 * @param traj Pointer to the trajectory to initialize
 * @return 0 on success, -1 on allocation failure
 */
int trajectory_init(Trajectory *traj);

/**
 * @brief Appends a point to the trajectory (resizes if needed).
 * @param traj Pointer to the trajectory
 * @param p    The point to append
 * @return 0 on success, -1 on allocation failure
 */
int trajectory_append(Trajectory *traj, Point p);

/**
 * @brief Frees the memory used by a trajectory.
 * @param traj Pointer to the trajectory
 */
void trajectory_free(Trajectory *traj);

/**
 * @brief Runs unit tests for trajectory operations (prints results to stdout).
 */
void trajectory_test(void);

#endif 