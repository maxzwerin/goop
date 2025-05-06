#include "FPToolkit.c"

#define MAX_RECS 1000
#define GRID_SIZE 20

int WIN_X = 800;
int WIN_Y = 800;

typedef struct {
    double x,y, w,h;
} Rec;

Rec rects[MAX_RECS];
int count = 0;

void snap_to_grid(double *x, double *y) {
    *x = round(*x / GRID_SIZE) * GRID_SIZE;
    *y = round(*y / GRID_SIZE) * GRID_SIZE;
}

void draw_rectangle(Rec r) {
    G_fill_rectangle(r.x, r.y, r.w, r.h);
}

void draw_grid() {
    Gi_rgb(0,50,100);
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
    for (int i = 0; i < count; i++) {
        draw_rectangle(rects[i]);
    }
    G_display_image();
}

void find_fourth_point(double x[], double y[]) {
    double x4, y4;

    if (x[0] == x[1]) x4 = x[2];
    else if (x[0] == x[2]) x4 = x[1];
    else x4 = x[0];

    if (y[0] == y[1]) y4 = y[2];
    else if (y[0] == y[2]) y4 = y[1];
    else y4 = y[0];

    x[3] = x4;
    y[3] = y4;
}

void wait_click_lines(double x[], double y[], int n) {
    if (n <= 0) return;
    int i;
    Gi_rgb(255,255,255);

    for (i = 0; i < n; i++) {
        G_line(x[i], y[i], x[i + 1], y[i + 1]);
    }

    for (i = 0; i <= n; i++) {
        G_circle(x[i], y[i], 5);
    }
}

void wait_and_click_rec() {
    double x[4], y[4];
    double p[2];
    int P[2];
    int i;

    for (i = 0; i < 3; i++) {
        while (!G_no_wait_event(p)) {
            S_mouse_coord_window(P);
            P[1] = WIN_Y - P[1];
            x[i] = P[0]; y[i] = P[1];

            draw_all();
            wait_click_lines(x,y,i);
            G_display_image();
        }

        snap_to_grid(&p[0],&p[1]);
        draw_all();
        x[i] = p[0];
        y[i] = p[1];
        wait_click_lines(x,y,i);
        G_display_image();
    }

    find_fourth_point(x,y);

    double min_x = x[0], max_x = x[0];
    double min_y = y[0], max_y = y[0];

    for (i = 1; i < 4; i++) {
        if (x[i] < min_x) min_x = x[i];
        if (x[i] > max_x) max_x = x[i];
        if (y[i] < min_y) min_y = y[i];
        if (y[i] > max_y) max_y = y[i];
    }

    rects[count].x = min_x;
    rects[count].y = min_y;
    rects[count].w = max_x - min_x;
    rects[count].h = max_y - min_y;

    count++;
}


int main() {
    G_init_graphics(WIN_X,WIN_Y);
    draw_all();
    wait_and_click_rec();
    count = 1;
    draw_all();
    int key = G_wait_key();
}


