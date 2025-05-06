#include "FPToolkit.c"

#define MAX_RECS 1000
#define GRID_SIZE 20

int WIN_X;
int WIN_Y;

void screen() {
    int real[2];
    int actual[2] = { 1 };
    int buffer = 50;
    G_get_max_screen(&real[0],&real[1]);
    real[0] -= buffer; real[1] -= buffer;
    WIN_X = real[0];
    WIN_Y = real[1];
    Resize_window_X(real,actual);
}


typedef struct {
    double x[4],y[4]; 
    double w,h;
} Rec;

Rec rec[MAX_RECS];
int count = 0;

void snap_to_grid(double *x, double *y) {
    *x = round(*x / GRID_SIZE) * GRID_SIZE;
    *y = round(*y / GRID_SIZE) * GRID_SIZE;
}

void draw_rectangle(Rec r) {
    G_fill_rectangle(r.x[0],r.y[0], r.w,r.h);
}

void draw_grid() {
    Gi_rgb(10,50,100);
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
    Gi_rgb(0,155,155);
    for (int i = 0; i < count; i++) {
        draw_rectangle(rec[i]);
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
    int i;
    Gi_rgb(255,255,255);

    for (i = 0; i < n; i++) {
        G_line(x[i], y[i], x[i + 1], y[i + 1]);
    }

    for (i = 0; i <= n; i++) {
        if (i == n) G_fill_circle(x[i],y[i], 5);
        else G_circle(x[i],y[i], 5);
    }
}

int compare(const void* a, const void* b) {
    return (*(double*)a - *(double*)b);
}

void convert_to_rec(double x[], double y[]) {
    int i,n;
    double min_x = x[0], max_x = x[0];
    double min_y = y[0], max_y = y[0];

    for (i = 1; i < 4; i++) {
        if (x[i] < min_x) min_x = x[i];
        if (x[i] > max_x) max_x = x[i];
        if (y[i] < min_y) min_y = y[i];
        if (y[i] > max_y) max_y = y[i];
    }

    for (i = 0; i < 4; i++) {
        rec[count].x[i] = x[i];
        rec[count].y[i] = y[i];
    }

    rec[count].w = max_x - min_x;
    rec[count].h = max_y - min_y;

    count++;
}

void wait_and_click_rec() {
    double x[4], y[4];
    double p[2];
    int P[2];
    int i,n;

    for (i = 0; i < 3; i++) {
        while (!G_no_wait_event(p)) {
            S_mouse_coord_window(P);
            P[1] = WIN_Y - P[1]; // comes in upside-down
            
            x[i] = (double) P[0]; 
            y[i] = (double) P[1];
            snap_to_grid(&x[i],&y[i]);
            
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

    n = sizeof(x) / sizeof(x[0]);
    qsort(x,n,sizeof(double),compare);
    qsort(y,n,sizeof(double),compare);

    convert_to_rec(x,y); // count++ here
}


void print_recs() {
    int i,j;
    printf("\n\nCOUNT: %d\n",count);
    
    for (i = 0; i < count; i++) {
        printf("--- ");
        for (j = 0; j < 4; j++) {
            if (j < 3) printf("{ %4.0lf, %4.0lf }, ",rec[i].x[j],rec[i].y[j]);
            else printf("{ %4.0lf, %4.0lf }\n",rec[i].x[j],rec[i].y[j]);
        }
        printf("+++ { WIDTH = %4.0lf , HEIGHT = %4.0lf }\n\n",rec[i].w,rec[i].h);
    }
}


int main() {
    int i, key;
    double p[2];
    G_init_graphics(1,1);
    screen();
    
    while (1) {
        draw_all();
        key = G_wait_event(p);
        
        if (key == 'a') wait_and_click_rec();
        if (key == 's') print_recs();
        if (key == 'q') break;
        if (key == 0)   i = 0; // TODO: has rec been clicked, return ID 
    }
    return 0;
}


