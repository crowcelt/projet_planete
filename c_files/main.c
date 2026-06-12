/**
 * @file main.c
 * @brief Simulation du système solaire complet avec la méthode RK2.
 *
 * Chaque planète est simulée sur 2 orbites complètes avec un pas de temps
 * adapté à sa période orbitale, pour éviter la divergence et limiter la
 * taille du fichier JSON.
 *
 * Pas de temps et steps choisis pour ~2 orbites avec ~1000 points par orbite.
 */

#include "../header/vector.h"
#include "../header/trajectory.h"
#include "../header/planet.h"
#include "../header/solvers.h"
#include "../header/energy.h"
#include "../header/export.h"

#include <stdio.h>

#define OUTPUT_FILE "output/trajectories.json"
#define NB_PLANETS 8

/*
 * Paramètres par planète : { dt (s), steps }
 * Période orbitale * 2 / dt = steps (~2000 points par planète)
 *
 * Mercure  : période 88j,   dt=4h    -> 2*88*6    = 1056 steps
 * Vénus    : période 225j,  dt=12h   -> 2*225*2   = 900  steps
 * Terre    : période 365j,  dt=12h   -> 2*365*2   = 1460 steps
 * Mars     : période 687j,  dt=1j    -> 2*687      = 1374 steps
 * Jupiter  : période 4333j, dt=5j   -> 2*4333/5   = 1733 steps
 * Saturne  : période 10759j,dt=10j  -> 2*10759/10 = 2152 steps
 * Uranus   : période 30687j,dt=30j  -> 2*30687/30 = 2046 steps
 * Neptune  : période 60190j,dt=60j  -> 2*60190/60 = 2006 steps
 */
static double DT[NB_PLANETS]    = {
    14400.0,   /* Mercure  : 4h */
    43200.0,   /* Venus    : 12h */
    43200.0,   /* Terre    : 12h */
    86400.0,   /* Mars     : 1j */
    432000.0,  /* Jupiter  : 5j */
    864000.0,  /* Saturne  : 10j */
    2592000.0, /* Uranus   : 30j */
    5184000.0  /* Neptune  : 60j */
};

static int STEPS[NB_PLANETS] = {
    1056,  /* Mercure */
    900,   /* Venus */
    1460,  /* Terre */
    1374,  /* Mars */
    1733,  /* Jupiter */
    2152,  /* Saturne */
    2046,  /* Uranus */
    2006   /* Neptune */
};

int main(void)
{
    Planet planets[NB_PLANETS];
    int i;

    /* ------------------------------------------------------------------
     * Initialisation des 8 planètes
     * ------------------------------------------------------------------ */
    if (planet_init(&planets[0], "mercury-RK2", MASS_MERCURY, PERIHELION_MERCURY, V0_MERCURY) != 0) {
        fprintf(stderr, "ERROR: planet_init mercury\n"); return 1;
    }
    if (planet_init(&planets[1], "venus-RK2", MASS_VENUS, PERIHELION_VENUS, V0_VENUS) != 0) {
        fprintf(stderr, "ERROR: planet_init venus\n"); return 1;
    }
    if (planet_init(&planets[2], "earth-RK2", MASS_EARTH, PERIHELION_EARTH, V0_EARTH) != 0) {
        fprintf(stderr, "ERROR: planet_init earth\n"); return 1;
    }
    if (planet_init(&planets[3], "mars-RK2", MASS_MARS, PERIHELION_MARS, V0_MARS) != 0) {
        fprintf(stderr, "ERROR: planet_init mars\n"); return 1;
    }
    if (planet_init(&planets[4], "jupiter-RK2", MASS_JUPITER, PERIHELION_JUPITER, V0_JUPITER) != 0) {
        fprintf(stderr, "ERROR: planet_init jupiter\n"); return 1;
    }
    if (planet_init(&planets[5], "saturn-RK2", MASS_SATURN, PERIHELION_SATURN, V0_SATURN) != 0) {
        fprintf(stderr, "ERROR: planet_init saturn\n"); return 1;
    }
    if (planet_init(&planets[6], "uranus-RK2", MASS_URANUS, PERIHELION_URANUS, V0_URANUS) != 0) {
        fprintf(stderr, "ERROR: planet_init uranus\n"); return 1;
    }
    if (planet_init(&planets[7], "neptune-RK2", MASS_NEPTUNE, PERIHELION_NEPTUNE, V0_NEPTUNE) != 0) {
        fprintf(stderr, "ERROR: planet_init neptune\n"); return 1;
    }

    /* ------------------------------------------------------------------
     * Simulation RK2 avec pas de temps adapté par planète
     * ------------------------------------------------------------------ */
    for (i = 0; i < NB_PLANETS; i++) {
        if (rk2_simulate(&planets[i], MASS_SUN, DT[i], STEPS[i]) != 0) {
            fprintf(stderr, "ERROR: rk2_simulate %s\n", planets[i].name);
            return 1;
        }
        printf("%s: %d points (dt=%.0fs)\n",
               planets[i].name, planets[i].trajectory.size, DT[i]);
    }

    /* ------------------------------------------------------------------
     * Export JSON
     * ------------------------------------------------------------------ */
    if (export_to_file(OUTPUT_FILE, planets, NB_PLANETS) != 0) {
        fprintf(stderr, "ERROR: export_to_file\n");
        return 1;
    }

    /* ------------------------------------------------------------------
     * Cleanup
     * ------------------------------------------------------------------ */
    for (i = 0; i < NB_PLANETS; i++) {
        planet_free(&planets[i]);
    }

    return 0;
}