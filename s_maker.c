#include "FPToolkit.c"

#define MAX_RECS 1000
#define GRID_SIZE 20

int WIN_X = 800;
int WIN_Y = 800;

typedef struct {
    double x, y, w, h;
} Rectangle;

Rectangle rects[MAX_RECS];
int rect_count = 0;

void snap_to_grid(double *x, double *y) {
    *x = round(*x / GRID_SIZE) * GRID_SIZE;
    *y = round(*y / GRID_SIZE) * GRID_SIZE;
}

void draw_rectangle(Rectangle r) {
    G_fill_rectangle(r.x, r.y, r.w, r.h);
}

void draw_grid() {
    Gi_rgb(50,50,50);
    for (int i = 0; i <= WIN_X; i += GRID_SIZE) {
        G_line(i, 0, i, WIN_Y);
    }
    for (int j = 0; j <= WIN_Y; j += GRID_SIZE) {
        G_line(0, j, WIN_X, j);
    }
}

void draw_all() {
    Gi_rgb(0,0,0);
    G_clear();
    draw_grid();
    Gi_rgb(55,125,100);
    for (int i = 0; i < rect_count; i++) {
        draw_rectangle(rects[i]);
    }
}

Rectangle create_rectangle_from_clicks() {
    double x0, y0, x1, y1;
    double p[2];

    G_wait_click(p); snap_to_grid(&p[0], &p[1]);
    x0 = p[0]; y0 = p[1];
    G_fill_circle(x0, y0, 3);

    G_wait_click(p); snap_to_grid(&p[0], &p[1]);
    x1 = p[0]; y1 = p[1];
    G_fill_circle(x1, y1, 3);

    double rx = fmin(x0, x1);
    double ry = fmin(y0, y1);
    double rw = fabs(x1 - x0);
    double rh = fabs(y1 - y0);

    Rectangle r = {rx, ry, rw, rh};
    return r;
}

int main() {
    G_init_graphics(WIN_X, WIN_Y);
    draw_all();

    while (rect_count < MAX_RECS) {
        Rectangle r = create_rectangle_from_clicks();
        rects[rect_count++] = r;

        draw_all();

        int key = G_wait_key();
        if (key == 'q') break;
    }

    // Print all rectangles
    printf("\nRectangles:\n");
    for (int i = 0; i < rect_count; i++) {
        Rectangle r = rects[i];
        printf("%d: (%.1f, %.1f, %.1f, %.1f)\n", i, r.x, r.y, r.w, r.h);
    }

    return 0;
}

