/**
 * @file solvers.c
 * @brief ODE integration methods: Euler, asymmetric Euler, and Runge-Kutta 2.
 */

#include "../header/solvers.h"
#include <math.h>
#include <stddef.h>

/* --------------------------------------------------------------------------
 * Shared helper
 * -------------------------------------------------------------------------- */

Vector compute_acceleration(Vector r_a, Vector r_b, double mass_b)
{
    Vector r;
    double norm;
    double coeff;

    r    = vector_sub(r_a, r_b);
    norm = vector_norm(r);

    if (norm == 0.0) {
        return vector_create(0.0, 0.0, 0.0);
    }

    coeff = -G_CONST * mass_b / (norm * norm * norm);
    return vector_scale(r, coeff);
}

/* --------------------------------------------------------------------------
 * Simple Euler
 * -------------------------------------------------------------------------- */

int euler_simulate(Planet *planet, double mass_sun, double dt, int steps)
{
    int    i;
    Point  current;
    Point  next;
    Vector r_sun = {0.0, 0.0, 0.0};
    Vector accel;

    if (planet == NULL || planet->trajectory.size == 0) {
        return -1;
    }

    for (i = 0; i < steps; i++) {
        current = planet->trajectory.points[planet->trajectory.size - 1];

        /* 1. Acceleration from current position */
        accel = compute_acceleration(current.r, r_sun, mass_sun);

        /* 2. r_{n+1} = r_n + v_n * dt */
        next.r = vector_add(current.r, vector_scale(current.v, dt));

        /* 3. v_{n+1} = v_n + a_n * dt */
        next.v = vector_add(current.v, vector_scale(accel, dt));

        next.t = current.t + 1;

        if (trajectory_append(&planet->trajectory, next) != 0) {
            return -1;
        }
    }

    return 0;
}

/* --------------------------------------------------------------------------
 * Asymmetric Euler
 * -------------------------------------------------------------------------- */

int euler_asym_simulate(Planet *planet, double mass_sun, double dt, int steps)
{
    int    i;
    Point  current;
    Point  next;
    Vector r_sun = {0.0, 0.0, 0.0};
    Vector accel;

    if (planet == NULL || planet->trajectory.size == 0) {
        return -1;
    }

    for (i = 0; i < steps; i++) {
        current = planet->trajectory.points[planet->trajectory.size - 1];

        /* 1. r_{n+1} = r_n + v_n * dt  (position first) */
        next.r = vector_add(current.r, vector_scale(current.v, dt));

        /* 2. Acceleration from NEW position */
        accel = compute_acceleration(next.r, r_sun, mass_sun);

        /* 3. v_{n+1} = v_n + a_{n+1} * dt */
        next.v = vector_add(current.v, vector_scale(accel, dt));

        next.t = current.t + 1;

        if (trajectory_append(&planet->trajectory, next) != 0) {
            return -1;
        }
    }

    return 0;
}

/* --------------------------------------------------------------------------
 * Runge-Kutta order 2
 * -------------------------------------------------------------------------- */

int rk2_simulate(Planet *planet, double mass_sun, double dt, int steps)
{
    int    i;
    Point  current;
    Point  next;
    Vector r_sun = {0.0, 0.0, 0.0};
    Vector k1_r, k1_v;
    Vector k2_r, k2_v;
    Vector r_mid;

    if (planet == NULL || planet->trajectory.size == 0) {
        return -1;
    }

    for (i = 0; i < steps; i++) {
        current = planet->trajectory.points[planet->trajectory.size - 1];

        k1_r = vector_scale(current.v, dt);
        k1_v = vector_scale(compute_acceleration(current.r, r_sun, mass_sun), dt);

        k2_r = vector_scale(vector_add(current.v, vector_scale(k1_v, 0.5)), dt);
        r_mid = vector_add(current.r, vector_scale(k1_r, 0.5));
        k2_v = vector_scale(compute_acceleration(r_mid, r_sun, mass_sun), dt);

        next.r = vector_add(current.r, k2_r);
        next.v = vector_add(current.v, k2_v);
        next.t = current.t + 1;

        if (trajectory_append(&planet->trajectory, next) != 0) {
            return -1;
        }
    }

    return 0;
}
