/**
 * @file main.c
 * @brief Simulation du système solaire complet — 3 méthodes, 3 fichiers JSON.
 *
 * Génère :
 *   output/solar_system_euler.json      -> 8 planètes, Euler simple
 *   output/solar_system_euler_asym.json -> 8 planètes, Euler asymétrique
 *   output/solar_system_rk2.json        -> 8 planètes, RK2
 */

#include "../header/vector.h"
#include "../header/trajectory.h"
#include "../header/planet.h"
#include "../header/solvers.h"
#include "../header/energy.h"
#include "../header/export.h"

#include <stdio.h>
#include <string.h>

#define NB_PLANETS 8

/* Pas de temps adapté par planète (en secondes) */
static double DT[NB_PLANETS] = {
    14400.0,    /* Mercure  : 4h  */
    43200.0,    /* Venus    : 12h */
    43200.0,    /* Terre    : 12h */
    86400.0,    /* Mars     : 1j  */
    432000.0,   /* Jupiter  : 5j  */
    864000.0,   /* Saturne  : 10j */
    2592000.0,  /* Uranus   : 30j */
    5184000.0   /* Neptune  : 60j */
};

/* ~2 orbites complètes par planète */
static int STEPS[NB_PLANETS] = {
    1056,   /* Mercure */
    900,    /* Venus   */
    1460,   /* Terre   */
    1374,   /* Mars    */
    1733,   /* Jupiter */
    2152,   /* Saturne */
    2046,   /* Uranus  */
    2006    /* Neptune */
};

/* Noms de base des planètes */
static const char *BASE_NAMES[NB_PLANETS] = {
    "mercury", "venus", "earth", "mars",
    "jupiter", "saturn", "uranus", "neptune"
};

/* Masses */
static double MASSES[NB_PLANETS] = {
    MASS_MERCURY, MASS_VENUS, MASS_EARTH,   MASS_MARS,
    MASS_JUPITER, MASS_SATURN, MASS_URANUS, MASS_NEPTUNE
};

/* Périhélies */
static double PERIHELIA[NB_PLANETS] = {
    PERIHELION_MERCURY, PERIHELION_VENUS, PERIHELION_EARTH,   PERIHELION_MARS,
    PERIHELION_JUPITER, PERIHELION_SATURN, PERIHELION_URANUS, PERIHELION_NEPTUNE
};

/* Vitesses initiales */
static double V0S[NB_PLANETS] = {
    V0_MERCURY, V0_VENUS, V0_EARTH,   V0_MARS,
    V0_JUPITER, V0_SATURN, V0_URANUS, V0_NEPTUNE
};

/**
 * @brief Simule les 8 planètes avec une méthode donnée et exporte en JSON.
 *
 * @param suffix   Suffixe ajouté au nom de chaque planète (ex: "-RK2")
 * @param method   0 = Euler, 1 = Euler asym, 2 = RK2
 * @param filepath Chemin du fichier JSON de sortie
 * @return 0 si succès, -1 si erreur
 */
int simulate_and_export(const char *suffix, int method, const char *filepath)
{
    Planet planets[NB_PLANETS];
    char   name[64];
    int    i;

    printf("\n--- %s ---\n", filepath);

    /* Init + simulation */
    for (i = 0; i < NB_PLANETS; i++) {
        snprintf(name, sizeof(name), "%s%s-dt%.0f", BASE_NAMES[i], suffix, DT[i]);

        if (planet_init(&planets[i], name, MASSES[i], PERIHELIA[i], V0S[i]) != 0) {
            fprintf(stderr, "ERROR: planet_init %s\n", name);
            return -1;
        }

        int ret = 0;
        if      (method == 0) ret = euler_simulate      (&planets[i], MASS_SUN, DT[i], STEPS[i]);
        else if (method == 1) ret = euler_asym_simulate (&planets[i], MASS_SUN, DT[i], STEPS[i]);
        else                  ret = rk2_simulate        (&planets[i], MASS_SUN, DT[i], STEPS[i]);

        if (ret != 0) {
            fprintf(stderr, "ERROR: simulate %s\n", name);
            return -1;
        }

        printf("  %s: %d points\n", planets[i].name, planets[i].trajectory.size);
    }

    /* Export */
    if (export_to_file(filepath, planets, NB_PLANETS) != 0) {
        fprintf(stderr, "ERROR: export %s\n", filepath);
        return -1;
    }

    /* Cleanup */
    for (i = 0; i < NB_PLANETS; i++) planet_free(&planets[i]);

    return 0;
}

int main(void)
{
    if (simulate_and_export("-euler",      0, "output/solar_system_euler.json")      != 0) return 1;
    if (simulate_and_export("-euler-asym", 1, "output/solar_system_euler_asym.json") != 0) return 1;
    if (simulate_and_export("-RK2",        2, "output/solar_system_rk2.json")        != 0) return 1;

    printf("\nFichiers generes dans output/\n");
    printf("  solar_system_euler.json      -> 8 planetes, Euler simple\n");
    printf("  solar_system_euler_asym.json -> 8 planetes, Euler asymetrique\n");
    printf("  solar_system_rk2.json        -> 8 planetes, RK2\n");

    return 0;
}