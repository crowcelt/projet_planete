#ifndef PLANET_H
#define PLANET_H

/**
 * @file planet.h
 * @brief Planet structure and initialization utilities.
 */

#include "trajectory.h" /* also defines Point */

/** Maximum length of a planet name string. */
#define PLANET_NAME_MAX 64

/**
 * @struct Planet
 * @brief Represents a celestial body with its physical data and trajectory.
 */
typedef struct {
    char       name[PLANET_NAME_MAX]; /**< Name (e.g. "earth-euler") */
    double     mass;                  /**< Mass (kg) */
    double     perihelion;            /**< Perihelion distance from Sun (m) */
    Trajectory trajectory;            /**< Computed trajectory */
} Planet;

/**
 * @brief Initializes a planet and sets its first trajectory point.
 *
 * The planet is placed at perihelion on the x-axis.
 * Initial velocity is perpendicular (along y-axis), computed from vis-viva.
 *
 * @param planet     Pointer to the planet to initialize
 * @param name       Name string
 * @param mass       Mass in kg
 * @param perihelion Perihelion distance in m
 * @param v0         Initial orbital speed at perihelion (m/s)
 * @return 0 on success, -1 on failure
 */
int planet_init(Planet *planet, const char *name, double mass,
                double perihelion, double v0);

/**
 * @brief Frees the memory used by a planet (its trajectory).
 * @param planet Pointer to the planet
 */
void planet_free(Planet *planet);

/**
 * @brief Runs unit tests for planet initialization (prints results to stdout).
 */
void planet_test(void);

/* ---------------------------------------------------------------------------
 * Known planet constants (SI units)
 * ------------------------------------------------------------------------- */

#define MASS_SUN     1.989e30  /**< Mass of the Sun (kg) */

#define MASS_EARTH   5.972e24  /**< Mass of Earth (kg) */
#define PERIHELION_EARTH  1.470e11  /**< Earth perihelion (m) */
#define V0_EARTH     3.029e4   /**< Earth speed at perihelion (m/s) */

#define MASS_MERCURY 3.301e23
#define PERIHELION_MERCURY 4.600e10
#define V0_MERCURY   5.898e4

#define MASS_VENUS   4.867e24
#define PERIHELION_VENUS  1.075e11
#define V0_VENUS     3.526e4

#define MASS_MARS    6.417e23
#define PERIHELION_MARS   2.067e11
#define V0_MARS      2.650e4

#endif /* PLANET_H */
