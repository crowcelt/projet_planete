/**
 * @file trajectory.c
 * @brief Implementation of Point and Trajectory structures.
 */

#include "../headers/trajectory.h"
#include <stdlib.h>
#include <stdio.h>

/* --------------------------------------------------------------------------
 * Point
 * -------------------------------------------------------------------------- */

Point point_create(Vector r, Vector v, int t)
{
    Point p;
    p.r = r;
    p.v = v;
    p.t = t;
    return p;
}

void point_test(void)
{
    Vector r = {1.470e11, 0.0, 0.0};
    Vector v = {0.0, 3.029e4, 0.0};
    Point  p = point_create(r, v, 0);

    printf("=== point_test ===\n");
    printf("r = (%e, %e, %e)\n", p.r.x, p.r.y, p.r.z);
    printf("v = (%e, %e, %e)\n", p.v.x, p.v.y, p.v.z);
    printf("t = %d\n", p.t);
    printf("==================\n");
}

/* --------------------------------------------------------------------------
 * Trajectory
 * -------------------------------------------------------------------------- */

int trajectory_init(Trajectory *traj)
{
    if (traj == NULL) {
        return -1;
    }

    traj->points = (Point *)malloc(TRAJECTORY_INIT_CAPACITY * sizeof(Point));
    if (traj->points == NULL) {
        return -1;
    }

    traj->size     = 0;
    traj->capacity = TRAJECTORY_INIT_CAPACITY;
    return 0;
}

int trajectory_append(Trajectory *traj, Point p)
{
    Point *new_points = NULL;

    if (traj == NULL || traj->points == NULL) {
        return -1;
    }

    if (traj->size >= traj->capacity) {
        new_points = (Point *)realloc(traj->points,
                                      traj->capacity * 2 * sizeof(Point));
        if (new_points == NULL) {
            return -1;
        }
        traj->points   = new_points;
        traj->capacity = traj->capacity * 2;
    }

    traj->points[traj->size] = p;
    traj->size++;
    return 0;
}

void trajectory_free(Trajectory *traj)
{
    if (traj == NULL) {
        return;
    }
    free(traj->points);
    traj->points   = NULL;
    traj->size     = 0;
    traj->capacity = 0;
}

void trajectory_test(void)
{
    Trajectory traj;
    int i;

    printf("=== trajectory_test ===\n");

    if (trajectory_init(&traj) != 0) {
        printf("ERROR: trajectory_init failed\n");
        return;
    }

    for (i = 0; i < 5; i++) {
        Vector r = {(double)i * 1e10, 0.0, 0.0};
        Vector v = {0.0, (double)i * 1e3, 0.0};
        Point  p = point_create(r, v, i);
        if (trajectory_append(&traj, p) != 0) {
            printf("ERROR: trajectory_append failed at i=%d\n", i);
            trajectory_free(&traj);
            return;
        }
    }

    printf("Appended 5 points. size=%d, capacity=%d\n",
           traj.size, traj.capacity);
    printf("First point: r=(%e, %e, %e)\n",
           traj.points[0].r.x, traj.points[0].r.y, traj.points[0].r.z);
    printf("Last  point: r=(%e, %e, %e)\n",
           traj.points[4].r.x, traj.points[4].r.y, traj.points[4].r.z);

    trajectory_free(&traj);
    printf("Memory freed.\n");
    printf("=======================\n");
}
