#define WIDTH 1440
#define HEIGHT 900
#define BORDER 20

typedef struct {
    double x,y, w,h;
} Obj;

Obj level1[] = {
    {140.0, 360.0,  160.0, 160.0},
    {-10.0, 0.0,    810.0, 100.0},
    {600.0, 0.0,    220.0, 320.0}
};

int num_level1_objects = 3;

void backdrop_1() {
    int y,r,g,b,flag;
    r = g = b = 150;
    flag = 1;
    for (y = HEIGHT; y >= 0; y--) {
        if (y %  5 == 0) r -= flag;
        if (y % 10 == 0) g -= flag;
        if (y % 25 == 0) b -= flag;
        
        Gi_rgb(r,g,b);
        G_line(0,y, WIDTH,y);
    }
}

Obj level2[] = {
    {-10.0,  0.0,   810.0,  80.0},
    {340.0, 60.0,   150.0, 220.0},
    {380.0,  0.0,   120.0, 660.0},
    {-10.0, 280.0,   90.0, 140.0},
    {240.0, 540.0,  240.0, 120.0},
    {280.0, 680.0,  420.0, 520.0},
    {700.0, 240.0,  110.0, 160.0}
};

int num_level2_objects = 7;

// background                             -- function
// rectangles for level (with hitbox)     -- Obj struct
// foreground (rectangles with no hitbox) -- function
