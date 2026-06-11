/**
 * @file main.c
 * @brief Entry point for the Kepler solar system simulation project.
 *
 * Runs:
 *   1. Unit tests for all modules (commented out once validated)
 *   2. Earth-Sun simulation with Euler, asymmetric Euler and RK2
 *   3. Energy conservation check for each method
 *   4. Export of all trajectories to the JSON exchange file
 */

#include "vector.h"
#include "trajectory.h"
#include "planet.h"
#include "solvers.h"
#include "energy.h"
#include "export.h"

#include <stdio.h>

/* Simulation parameters */
#define DT_SECONDS   86400.0   /* 1 day in seconds */
#define STEPS        365       /* Simulate ~1 year */
#define OUTPUT_FILE  "output/trajectories.json"

int main(void)
{
    Planet earth_euler;
    Planet earth_asym;
    Planet earth_rk2;
    Planet planets[3];
    Vector r_sun = {0.0, 0.0, 0.0};

    /*
     * Unit tests — comment out once validated
     */
    vector_test();
    point_test();
    trajectory_test();
    planet_test();

    /* 
     * Euler simulation
     */
    if (planet_init(&earth_euler, "earth-euler",
                    MASS_EARTH, PERIHELION_EARTH, V0_EARTH) != 0) {
        fprintf(stderr, "ERROR: planet_init earth-euler\n");
        return 1;
    }
    if (euler_simulate(&earth_euler, MASS_SUN, DT_SECONDS, STEPS) != 0) {
        fprintf(stderr, "ERROR: euler_simulate\n");
        return 1;
    }
    printf("Euler:     %d points computed.\n", earth_euler.trajectory.size);

    /* 
     * Asymmetric Euler simulation
     */
    if (planet_init(&earth_asym, "earth-euler-asym",
                    MASS_EARTH, PERIHELION_EARTH, V0_EARTH) != 0) {
        fprintf(stderr, "ERROR: planet_init earth-euler-asym\n");
        return 1;
    }
    if (euler_asym_simulate(&earth_asym, MASS_SUN, DT_SECONDS, STEPS) != 0) {
        fprintf(stderr, "ERROR: euler_asym_simulate\n");
        return 1;
    }
    printf("Euler asym: %d points computed.\n", earth_asym.trajectory.size);

    /*
     * RK2 simulation
     */
    if (planet_init(&earth_rk2, "earth-RK2",
                    MASS_EARTH, PERIHELION_EARTH, V0_EARTH) != 0) {
        fprintf(stderr, "ERROR: planet_init earth-RK2\n");
        return 1;
    }
    if (rk2_simulate(&earth_rk2, MASS_SUN, DT_SECONDS, STEPS) != 0) {
        fprintf(stderr, "ERROR: rk2_simulate\n");
        return 1;
    }
    printf("RK2:        %d points computed.\n", earth_rk2.trajectory.size);

    /*
     * Energy conservation check (prints to stdout)
     **/
    /* Uncomment to see detailed energy tables: */
    /* energy_check(&earth_euler, MASS_SUN, r_sun); */
    /* energy_check(&earth_asym,  MASS_SUN, r_sun); */
    /* energy_check(&earth_rk2,   MASS_SUN, r_sun); */
    (void)r_sun; /* suppress unused warning when checks are commented out */

    /* 
     * Export to JSON
     **/
    planets[0] = earth_euler;
    planets[1] = earth_asym;
    planets[2] = earth_rk2;

    if (export_to_file(OUTPUT_FILE, planets, 3) != 0) {
        fprintf(stderr, "ERROR: export_to_file\n");
        return 1;
    }

    /* 
    * Cleanup
    **/
    planet_free(&earth_euler);
    planet_free(&earth_asym);
    planet_free(&earth_rk2);

    return 0;
}
