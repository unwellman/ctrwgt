#ifndef WORLD_H
#define WORLD_H

struct tile;
struct chunk;

void coord_world_tile (double *dst_x, double *dst_y, size_t i, size_t j);

void nearest_tile (size_t *dst_i, size_t *dst_j, double x, double y);

#endif

