#ifndef EXPORT_H
#define EXPORT_H

/**
 * @file export.h
 * @brief Export trajectories to the project's fixed JSON exchange format.
 *
 * Format example:
 * {
 *   "earth-euler": [
 *     [[1.470000e+11, 0.000000e+00, 0.000000e+00],
 *      [0.000000e+00, 3.028629e+04, 0.000000e+00], 0],
 *     ...
 *   ]
 * }
 */

#include "planet.h"
#include <stdio.h>

/**
 * @brief Writes a single planet's trajectory to an already-open file.
 *
 * @param file      Open file handle (write mode)
 * @param planet    Planet whose trajectory to export
 * @param is_last   1 if this is the last planet (no trailing comma), 0 otherwise
 */
void export_planet(FILE *file, const Planet *planet, int is_last);

/**
 * @brief Exports an array of planets to a JSON file at the given path.
 *
 * Creates (or overwrites) the file and writes all trajectories.
 *
 * @param filepath  Path to the output JSON file
 * @param planets   Array of planets
 * @param count     Number of planets in the array
 * @return 0 on success, -1 on file error
 */
int export_to_file(const char *filepath, const Planet *planets, int count);

#endif /* EXPORT_H */
