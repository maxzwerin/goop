#include "FPToolkit.c"
#include "levels.c"

#define N 10
#define RADIUS 40
#define WIDTH 1440
#define HEIGHT 900
#define STEP 0.01
#define JUMP 420.0

typedef struct {
    double x,y;
    double vx,vy;
} Point;

Point points[N+3];
double rest_length[N][N];
double stiffness = 0.015;
double damping = 0.98;
double gravity = 0.2;

double base_rest_length[N][N];
double rest_scale = 1.0;
double target_rest_scale = 1.0;

Obj *level_objects = level1;
int num_objects = 0;

int level = 1;
int debug = 0;

void init_points() {
    int i,j;
    double cx = 200;
    double cy = 200;
    for (i = 0; i < N; i++) {
        double angle = 2 * M_PI * i / N;
        points[i].x = cx + RADIUS * cos(angle);
        points[i].y = cy + RADIUS * sin(angle);
        points[i].vx = 0;
        points[i].vy = 0;
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (i != j) {
                double dx = points[j].x - points[i].x;
                double dy = points[j].y - points[i].y;
                rest_length[i][j] = sqrt(dx * dx + dy * dy);
            } else {
                rest_length[i][j] = 0;
            }
            base_rest_length[i][j] = rest_length[i][j];
        }
    }
}


int b_spline(double px[], double py[]) {
    double a[4],b[4],c[4],d[4],M[4];
    double t,t2,t3;
    int i,j,pc;
    double X,Y;

    a[0] = 1.0/6;    b[0] = 2.0/3;    c[0] = 1.0/6;    d[0] = 0.0;
    a[1] = -0.5;     b[1] = 0.0;      c[1] = 0.5;      d[1] = 0.0;
    a[2] = 0.5;      b[2] = -1.0;     c[2] = 0.5;      d[2] = 0.0;
    a[3] = -1.0/6;   b[3] = 0.5;      c[3] = -0.5;     d[3] = 1.0/6;

    pc = 0;

    for (i = 0; i < N; i++) {
        for (t = 0.0; t <= 1.0; t += STEP) {
            t2 = t * t;
            t3 = t2 * t;

            M[0] = a[0] + a[1]*t + a[2]*t2 + a[3]*t3;
            M[1] = b[0] + b[1]*t + b[2]*t2 + b[3]*t3;
            M[2] = c[0] + c[1]*t + c[2]*t2 + c[3]*t3;
            M[3] = d[0] + d[1]*t + d[2]*t2 + d[3]*t3;

            X = Y = 0;
            for (j = 0; j < 4; j++) {
                // idx wraps around when i+j == N
                int idx = (i + j) % N;
                X += M[j] * points[idx].x;
                Y += M[j] * points[idx].y;
            }
            px[pc] = X; py[pc] = Y; pc++;
        }
    }
    return pc;
}


void draw_level_objects() {
    for (int i = 0; i < num_objects; i++) {
        Obj r = level_objects[i];
        Gi_rgb(0,0,0);
        G_fill_rectangle(r.x, r.y, r.w, r.h);
    }
}


void draw() {
    int arr = N / STEP;
    double px[arr], py[arr];

    backdrop_1();
    draw_level_objects();

    int n = b_spline(px,py);
    Gi_rgb(155,55,50);
    G_fill_polygon(px,py,n);
    Gi_rgb(0,0,0);
    G_polygon(px,py,n);

    if (debug) {
        for (int i = 0; i < N; i++) {
            Gi_rgb(55,255,55);
            G_fill_circle(points[i].x, points[i].y, 2);
            for (int j = i + 1; j < N; j++) {
                Gi_rgb(255,255,255);
                G_line(points[i].x, points[i].y, points[j].x, points[j].y);
            }
        }
    }

    G_display_image();
}


void set_level(int new_level) {
    level = new_level;

    switch (level) {
        case 1:
            level_objects = level1;
            num_objects = num_level1_objects;
            break;
        case 2:
            level_objects = level2;
            num_objects = num_level2_objects;
            break;
        default:
            level_objects = level1;
            num_objects = num_level1_objects;
            break;
    }

    init_points();
}


void apply_spring_forces() {
    double dx,dy,dist;
    double rest,force;
    double fx,fy;
    double min_length = 10.0;

    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            dx = points[j].x - points[i].x;
            dy = points[j].y - points[i].y;
            dist = sqrt(dx * dx + dy * dy);

            // if (dist < min_length) dist = min_length;
            if (dist == 0) continue;

            rest = rest_length[i][j];
            force = stiffness * (dist - rest);

            fx = force * dx / dist;
            fy = force * dy / dist;

            points[i].vx += fx;
            points[i].vy += fy;
            points[j].vx -= fx;
            points[j].vy -= fy;
        }
    }
}


void resolve_collisions(Point *p) {
    for (int i = 0; i < num_objects; i++) {
        Obj r = level_objects[i];

        if (p->x >= r.x && p->x <= r.x + r.w &&
            p->y >= r.y && p->y <= r.y + r.h) {

            // push point out of the rectangle
            double left   = fabs(p->x - r.x);
            double right  = fabs((r.x + r.w) - p->x);
            double top    = fabs((r.y + r.h) - p->y);
            double bottom = fabs(p->y - r.y);

            double min_d = fmin(fmin(left, right), fmin(top, bottom));

            if (min_d == left) {
                p->x = r.x - 0.1;
                p->vx = 0;
            } else if (min_d == right) {
                p->x = r.x + r.w + 0.1;
                p->vx = 0;
            } else if (min_d == bottom) {
                p->y = r.y + 0.1;
                p->vy = 0;
            } else {
                p->y = r.y + r.h - 0.1;
                p->vy = 0;
            }
        }
    }
}


void apply_gravity() {
    for (int i = 0; i < N; i++) {
        points[i].vy -= gravity;
    }
}


void displace() {
    double s = 0.5;
    for (int i = 1; i < N; i++) {
        for (int j = i + 1; j < N - 1; j++) {
            if (fabs(points[i].y - points[j].y) < 1.0) {
                points[i].y += (rand() % 10 - 5) * s;
                points[j].y += (rand() % 10 - 5) * s;
            }
        }
    }
}


void update_points() {
    for (int i = 0; i < N; i++) {
        points[i].vx *= damping;
        points[i].vy *= damping;

        points[i].x += points[i].vx;
        points[i].y += points[i].vy;

        resolve_collisions(&points[i]);

        // edging 
        if (points[i].x < 0) {
            points[i].x = 0;
            points[i].vx = 0;
        }
        if (points[i].x > WIDTH) {
            points[i].x = WIDTH;
            points[i].vx = 0;
        }
        if (points[i].y < 0) {
            points[i].y = 0;
            points[i].vy = 0;
        }
    }

    // check for points at the same y-level and apply a
    // small displacement to avoid circle becoming a line
    int sum = 0;
    for (int i = 0; i < N - 1; i++) {
        if (fabs(points[i].y - points[i+1].y) < 1.0) sum++;
    }
    if (sum >= N - 2) displace();
}


void move(double dx, double dy) {
    for (int i = 0; i < N; i++) {
        points[i].x += dx;
        points[i].y += dy;
    }
}


int is_on_ground() {
    int grounded = 0;

    for (int i = 0; i < N; i++) {
        Point p = points[i];

        // Check bottom of window
        if (p.y <= 2.0) {
            grounded++;
            continue;
        }

        // Check if point is standing on any object
        for (int j = 0; j < num_objects; j++) {
            Obj r = level_objects[j];

            int within_x = (p.x >= r.x) && (p.x <= r.x + r.w);
            int on_top   = fabs(p.y - (r.y + r.h)) <= 2.0;

            if (within_x && on_top) {
                grounded++;
                break;
            }
        }

       if (grounded >= 2) return 1;
    }

    return 0;
}


int main() {
    int i,j;
    G_init_graphics(WIDTH, HEIGHT);
    set_level(1);

    double jump_velocity = sqrt(2 * gravity * JUMP);
    int jump_requested = 0;


    while (1) {
        if (G_key_down('w') == 'w') jump_requested = 1;
        else                        jump_requested = 0;

        if (G_key_down('a') == 'a') move(-5,0);
        if (G_key_down('d') == 'd') move(5, 0);

        if (G_key_press()   == 'b') debug = (debug) ? 0 : 1;
        if (G_key_down('q') == 'q') break;

        if (G_key_down(' ') == ' ') {
            target_rest_scale = 1.6;
        } else if (G_key_down('s') == 's') {
            target_rest_scale = 0.8;
            move(0,-5);
        } else target_rest_scale = 1.0;

        if (G_key_down('1') == '1') set_level(1);
        if (G_key_down('2') == '2') set_level(2);

        if (jump_requested && is_on_ground()) {
            for (i = 0; i < N; i++) {
                points[i].vy = jump_velocity;
            }
        }

        rest_scale = 0.9 * rest_scale + 0.1 * target_rest_scale;

        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                rest_length[i][j] = rest_scale * base_rest_length[i][j];
            }
        }

        apply_gravity();
        apply_spring_forces();
        update_points();
        draw();

        usleep(15000);
    }
    return 0;
}
