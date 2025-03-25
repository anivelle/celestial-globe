#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <libnova/libnova.h>

// #define NUM_STARS 118218
#define STAR_DIST 100
#define MIN_RADIUS 5.0 / STAR_DIST
#define MAX_RADIUS 20 * MIN_RADIUS

typedef struct star_data {
    double ra;  // Right ascension in radians
    double dec; // Declination in radians
    double vis_mag;
    char *name; // Star name (if available)
} star_data_t;

char render_table[1 << 17];
// star_data_t stars[NUM_STARS];

int main(int argc, char *argv[]) {
    InitWindow(1366, 768, "Celestial Globe");

    Camera3D camera = {0};
    camera.position = (Vector3){0.0, 0.0, 0.0};
    camera.up = (Vector3){0.0, 1.0, 0.0};
    camera.target = (Vector3){1.0, 0.0, 0.0};
    camera.fovy = 60.0;
    camera.projection = CAMERA_PERSPECTIVE;
    FILE *hipparcos = fopen("hip_main.dat", "r");
    // FILE *orion = fopen("orion.csv", "r");
    // FILE *ursa_maj = fopen("ursa_major.csv", "r");
    FILE *constellations = fopen("constellations.csv", "r");
    DisableCursor();

    // Parse star data and load the array with more easily-drawn values
    char *line = NULL;
    int num_stars = 0;
    size_t len;

    while (getline(&line, &len, constellations) > 0) {
        unsigned index = (unsigned)strtol(line, NULL, 10);
        // printf("%b\n", index & mask);
        // printf("%d", hash_table[hash(index)]);
        render_table[index] = 1;
        free(line);
        line = NULL;
        num_stars++;
    }

    printf("%d\n", num_stars);

    star_data_t *stars[num_stars];

    int hip_stars = 0;
    while (getline(&line, &len, hipparcos) > 0) {
        char *token = strtok(line, "|");
        unsigned id = (unsigned)strtol(token, NULL, 10);

        if (!render_table[id]) {
            continue;
        }
        star_data_t *star = malloc(sizeof(star_data_t));
        stars[hip_stars] = star;
        hip_stars++;

        star->name = ""; // strtok(NULL, "|");

        // Skip the second column, I just haven't removed them yet
        token = strtok(NULL, "|");
        char *ra_str = strtok(NULL, "|");
        char *dec_str = strtok(NULL, "|");
        star->vis_mag = strtod(strtok(NULL, "|"), NULL);

        // Convert string-based HMS and DMS to radians
        struct ln_hms ra_hms = {0};
        ra_hms.hours = (unsigned short)strtol(strtok(ra_str, " "), NULL, 10);
        ra_hms.minutes = (unsigned short)strtol(strtok(NULL, " "), NULL, 10);
        ra_hms.seconds = strtod(strtok(NULL, " "), NULL);
        star->ra = 2 * PI - ln_hms_to_rad(&ra_hms);

        struct ln_dms dec_dms = {0};
        long degrees = strtol(strtok(dec_str, " "), NULL, 10);
        if (degrees < 0) {
            degrees *= -1;
            dec_dms.neg = 1;
        }
        dec_dms.degrees = (unsigned short)degrees;
        dec_dms.minutes = (unsigned short)strtol(strtok(NULL, " "), NULL, 10);
        dec_dms.seconds = strtod(strtok(NULL, " "), NULL);
        star->dec = ln_dms_to_rad(&dec_dms);
        free(line);
        line = NULL;
        star = NULL;
    }
    printf("%d\n", hip_stars);

    /* 
     * Instancing code that I'm not really using but think could be cool?
     */
    // Matrix *transforms = (Matrix *)RL_CALLOC(hip_stars, sizeof(Matrix));
    // Mesh sphere = GenMeshSphere(1.0, 10, 10);
    // Material matInstances = LoadMaterialDefault();
    // matInstances.maps[MATERIAL_MAP_DIFFUSE].color = RAYWHITE;

    // The array can hold num_stars amount of stars but i contains the
    // number of stars that we actually found in the catalogue (for some
    // reason the data is missing 104 stars or so?
    // for (int j = 0; j < hip_stars; j++) {
    //     if (stars[j] == NULL)
    //         continue;
    //
    //     star_data_t star = *stars[j];
    //     // Calculate position and scaling
    //     double x = STAR_DIST * cos(star.dec) * cos(star.ra);
    //     double z = STAR_DIST * cos(star.dec) * sin(star.ra);
    //     double y = STAR_DIST * sin(star.dec);
    //     double radius = 1 / exp(star.vis_mag);
    //     radius = radius < MIN_RADIUS ? MIN_RADIUS : radius;
    //     radius = radius > MAX_RADIUS ? MAX_RADIUS : radius;
    //
    //     // Calculate 
    //     Matrix translation = MatrixTranslate(x, y, z);
    //     Matrix scale = MatrixScale(radius, radius, radius);
    //     transforms[j] = MatrixMultiply(scale, translation);
    // }
    // free(line);
    SetTargetFPS(144);

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_FREE);
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
          for (int j = 0; j < hip_stars; j++) {
              if (stars[j] == NULL)
                  continue;

              star_data_t star = *stars[j];
              // Calculate position and scaling
              double x = STAR_DIST * cos(star.dec) * cos(star.ra);
              double z = STAR_DIST * cos(star.dec) * sin(star.ra);
              double y = STAR_DIST * sin(star.dec);
              double radius = 1 / exp(star.vis_mag);
              radius = radius < MIN_RADIUS ? MIN_RADIUS : radius;
              radius = radius > MAX_RADIUS ? MAX_RADIUS : radius;
              Vector3 pos = {x, y, z};
              DrawSphereEx(pos, radius, 5, 5, RAYWHITE);
          }
          // DrawMeshInstanced(sphere, matInstances, transforms, hip_stars);
        EndMode3D();
        EndDrawing();
    }
    fclose(hipparcos);
    fclose(constellations);
    // RL_FREE(transforms);
    // fclose(ursa_maj);
    // fclose(orion);
    for (int j = 0; j < hip_stars; j++) {
        free(stars[j]);
        stars[hip_stars] = NULL;
    }
    CloseWindow();
    return 0;
}
