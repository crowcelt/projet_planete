/**
 * @file planet.c
 * @brief Implementation of the Planet structure and initialization.
 */

#include "../header/planet.h"
#include <string.h>
#include <stdio.h>
#include "../header/planet.h"
#include <stdio.h>

int planet_init(Planet *planet, const char *name, double mass,
                double perihelion, double v0)
{
    Point  initial_point;
    Vector r0;
    Vector v0_vec;

    if (planet == NULL || name == NULL) {
        return -1;
    }

    strncpy(planet->name, name, PLANET_NAME_MAX - 1);
    planet->name[PLANET_NAME_MAX - 1] = '\0';

    planet->mass      = mass;
    planet->perihelion = perihelion;

    /* Place the planet at perihelion on the x-axis (point P in the subject) */
    r0 = vector_create(perihelion, 0.0, 0.0);

    /* Velocity is perpendicular to r, so along y-axis */
    v0_vec = vector_create(0.0, v0, 0.0);

    /* Initialize trajectory and add first point at t=0 */
    if (trajectory_init(&planet->trajectory) != 0) {
        return -1;
    }

    initial_point = point_create(r0, v0_vec, 0);
    if (trajectory_append(&planet->trajectory, initial_point) != 0) {
        trajectory_free(&planet->trajectory);
        return -1;
    }

    return 0;
}

void planet_free(Planet *planet)
{
    if (planet == NULL) {
        return;
    }
    trajectory_free(&planet->trajectory);
}

void planet_test(void)
{
    Planet earth;

    printf("=== planet_test ===\n");

    if (planet_init(&earth, "earth-test", MASS_EARTH,
                    PERIHELION_EARTH, V0_EARTH) != 0) {
        printf("ERROR: planet_init failed\n");
        return;
    }

    printf("Name       : %s\n", earth.name);
    printf("Mass       : %e kg\n", earth.mass);
    printf("Perihelion : %e m\n", earth.perihelion);
    printf("Initial r  : (%e, %e, %e)\n",
           earth.trajectory.points[0].r.x,
           earth.trajectory.points[0].r.y,
           earth.trajectory.points[0].r.z);
    printf("Initial v  : (%e, %e, %e)\n",
           earth.trajectory.points[0].v.x,
           earth.trajectory.points[0].v.y,
           earth.trajectory.points[0].v.z);
    printf("t=0        : %f\n", earth.trajectory.points[0].t);

    planet_free(&earth);
    printf("Memory freed.\n");
    printf("===================\n");
}