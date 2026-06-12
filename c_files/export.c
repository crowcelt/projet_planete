/**
 * @file export.c
 * @brief Export trajectories to the project's fixed JSON exchange format.
 */

#include "../header/export.h"
#include <stdio.h>

void export_planet(FILE *file, const Planet *planet, int is_last)
{
    int   i;
    Point p;

    if (file == NULL || planet == NULL) {
        return;
    }

    fprintf(file, "\"%s\" : [\n", planet->name);

    for (i = 0; i < planet->trajectory.size; i++) {
        p = planet->trajectory.points[i];

        /* Format: [[rx, ry, rz],[vx, vy, vz], t] */
        fprintf(file,
                "[[%e, %e, %e],[%e, %e, %e], %d]",
                p.r.x, p.r.y, p.r.z,
                p.v.x, p.v.y, p.v.z,
                p.t);

        /* Comma after each point except the last */
        if (i < planet->trajectory.size - 1) {
            fprintf(file, ",\n");
        } else {
            fprintf(file, "\n");
        }
    }

    fprintf(file, "]");

    /* Comma between planets */
    if (!is_last) {
        fprintf(file, ",\n");
    } else {
        fprintf(file, "\n");
    }
}

int export_to_file(const char *filepath, const Planet *planets, int count)
{
    int   i;
    FILE *file = NULL;

    if (filepath == NULL || planets == NULL || count <= 0) {
        return -1;
    }

    file = fopen(filepath, "w");
    if (file == NULL) {
        fprintf(stderr, "ERROR: cannot open file '%s' for writing\n", filepath);
        return -1;
    }

    fprintf(file, "{\n");

    for (i = 0; i < count; i++) {
        export_planet(file, &planets[i], i == count - 1);
    }

    fprintf(file, "}\n");

    fclose(file);
    printf("Exported %d trajectory(ies) to '%s'\n", count, filepath);
    return 0;
}
