/**
 * @file energy.c
 * @brief Energy computation and conservation verification.
 */

#include "energy.h"
#include <math.h>
#include <stdio.h>

double energy_kinetic(double mass, Vector v)
{
    double norm = vector_norm(v);
    return 0.5 * mass * norm * norm;
}

double energy_potential(double mass1, double mass2, Vector r1, Vector r2)
{
    Vector diff = vector_sub(r1, r2);
    double dist = vector_norm(diff);

    if (dist == 0.0) {
        return 0.0;
    }

    return -G_CONST * mass1 * mass2 / dist;
}

void energy_check(const Planet *planet, double mass_sun, Vector r_sun)
{
    int    i;
    double ec;
    double ep;
    double e_total;
    Point  p;

    if (planet == NULL) {
        return;
    }

    printf("=== energy_check: %s ===\n", planet->name);
    printf("%-8s %-20s %-20s %-20s\n", "step", "Ec (J)", "Ep (J)", "E_total (J)");

    for (i = 0; i < planet->trajectory.size; i++) {
        p  = planet->trajectory.points[i];
        ec = energy_kinetic(planet->mass, p.v);
        ep = energy_potential(planet->mass, mass_sun, p.r, r_sun);
        e_total = ec + ep;
        printf("%-8d %-20.6e %-20.6e %-20.6e\n", p.t, ec, ep, e_total);
    }

    printf("========================\n");
}
