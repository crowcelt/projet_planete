#ifndef ENERGY_H
#define ENERGY_H

/**
 * @file energy.h
 * @brief Functions to compute and verify energy conservation.
 *
 * The total mechanical energy E = Ec + Ep must remain constant over time
 * for a correct numerical integration.
 */

#include "planet.h"
#include "solvers.h" /* for G_CONST */

/**
 * @brief Computes the kinetic energy of a single body at a given point.
 *
 * Ec = 0.5 * m * ||v||^2
 *
 * @param mass  Mass of the body (kg)
 * @param v     Velocity vector (m/s)
 * @return Kinetic energy (J)
 */
double energy_kinetic(double mass, Vector v);

/**
 * @brief Computes the gravitational potential energy between two bodies.
 *
 * Ep = -G * m1 * m2 / ||r1 - r2||
 *
 * @param mass1 Mass of body 1 (kg)
 * @param mass2 Mass of body 2 (kg)
 * @param r1    Position of body 1 (m)
 * @param r2    Position of body 2 (m)
 * @return Potential energy (J)
 */
double energy_potential(double mass1, double mass2, Vector r1, Vector r2);

/**
 * @brief Computes and prints the total energy at each step of a planet's trajectory.
 *
 * Prints: step, Ec, Ep, E_total for each point in the trajectory.
 * Useful to visually verify that E_total stays approximately constant.
 *
 * @param planet    The simulated planet
 * @param mass_sun  Mass of the Sun (kg)
 * @param r_sun     Fixed position of the Sun (origin in heliocentric frame)
 */
void energy_check(const Planet *planet, double mass_sun, Vector r_sun);

#endif /* ENERGY_H */
