#ifndef SOLVERS_H
#define SOLVERS_H

/**
 * @file solvers.h
 * @brief ODE integration methods: Euler, asymmetric Euler, and Runge-Kutta 2.
 */

#include "planet.h"

/** Gravitational constant (N·m²·kg⁻²) */
#define G_CONST 6.67408e-11

/**
 * @brief Computes the gravitational acceleration on body A due to body B.
 *
 * a = -G * mB / ||r||^3 * r
 *
 * @param r_a    Position of body A
 * @param r_b    Position of body B (attractor)
 * @param mass_b Mass of body B (kg)
 * @return Acceleration vector (m/s²)
 */
Vector compute_acceleration(Vector r_a, Vector r_b, double mass_b);

/**
 * @brief Simple Euler integration.
 *
 * Per step:
 *   1. a = accel(r_n)
 *   2. r_{n+1} = r_n + v_n * dt
 *   3. v_{n+1} = v_n + a * dt
 *
 * @param planet   Planet to simulate
 * @param mass_sun Mass of the Sun (kg)
 * @param dt       Time step (s)
 * @param steps    Number of steps
 * @return 0 on success, -1 on error
 */
int euler_simulate(Planet *planet, double mass_sun, double dt, int steps);

/**
 * @brief Asymmetric (symplectic) Euler integration — better energy conservation.
 *
 * Per step:
 *   1. r_{n+1} = r_n + v_n * dt
 *   2. a = accel(r_{n+1})
 *   3. v_{n+1} = v_n + a * dt
 *
 * @param planet   Planet to simulate
 * @param mass_sun Mass of the Sun (kg)
 * @param dt       Time step (s)
 * @param steps    Number of steps
 * @return 0 on success, -1 on error
 */
int euler_asym_simulate(Planet *planet, double mass_sun, double dt, int steps);

/**
 * @brief Runge-Kutta order 2 (midpoint method).
 *
 * Per step:
 *   k1_r = dt * v_n
 *   k1_v = dt * a(r_n)
 *   k2_r = dt * (v_n + k1_v/2)
 *   k2_v = dt * a(r_n + k1_r/2)
 *   r_{n+1} = r_n + k2_r
 *   v_{n+1} = v_n + k2_v
 *
 * @param planet   Planet to simulate
 * @param mass_sun Mass of the Sun (kg)
 * @param dt       Time step (s)
 * @param steps    Number of steps
 * @return 0 on success, -1 on error
 */
int rk2_simulate(Planet *planet, double mass_sun, double dt, int steps);

#endif /* SOLVERS_H */
