#include <stdio.h>
#include <math.h>
#include <raylib.h>

#define WIDTH 1000
#define HEIGHT 1000
#define FPS 60
#define RADIUS 15.0f
#define NUM_POINTS 3*5000

typedef struct State {
    float x1_pos, y1_pos, x2_pos, y2_pos, x3_pos, y3_pos;
    float x1_speed, y1_speed, x2_speed, y2_speed, x3_speed, y3_speed;
    float x1_acc, y1_acc, x2_acc, y2_acc, x3_acc, y3_acc;
} State;

typedef struct Params {
    float G;                // Gravitationskonstante
    float m1, m2, m3;       // Massen
    float dt;               // Zeitschritt
} Params;

typedef struct Trace{
    Vector2 points[NUM_POINTS];
    int count;
}Trace;

void verletSpeed(State* s, Params* p);
void calcAcceleration(State* s, Params* p);
float dist(float x1, float x2, float y1, float y2);
void draw(const State* s, Trace* t);

int main() {
    InitWindow(WIDTH, HEIGHT, "Three Body Problem");
    SetTargetFPS(FPS);

    float centerX = WIDTH / 2.0f;
    float centerY = HEIGHT / 2.0f;
    float spawnRadius = 150.0f;

    float x1 = centerX + GetRandomValue(-spawnRadius, spawnRadius);
    float y1 = centerY + GetRandomValue(-spawnRadius, spawnRadius);
    float x2 = centerX + GetRandomValue(-spawnRadius, spawnRadius);
    float y2 = centerY + GetRandomValue(-spawnRadius, spawnRadius);
    float x3 = centerX + GetRandomValue(-spawnRadius, spawnRadius);
    float y3 = centerY + GetRandomValue(-spawnRadius, spawnRadius);

    State s = { 
        .x1_pos = x1, .y1_pos = y1,
        .x2_pos = x2, .y2_pos = y2,
        .x3_pos = x3, .y3_pos = y3,
        .x1_speed = 1.0f,  .y1_speed = 0.0f,
        .x2_speed = -0.5f, .y2_speed = 0.8f,
        .x3_speed = -0.5f, .y3_speed = -0.8f,
        .x1_acc = 0.0f, .y1_acc = 0.0f,
        .x2_acc = 0.0f, .y2_acc = 0.0f,
        .x3_acc = 0.0f, .y3_acc = 0.0f
    };

    Params p = { 
        .G = 15.0f,
        .m1 = 20, .m2 = 21, .m3 = 22,
        .dt = 0.0099f
    };

    Trace t = {.count = 0};

    while(!WindowShouldClose()) {
        for(int i = 0; i < 200; i++){
            verletSpeed(&s, &p);
        }
        
        BeginDrawing();

        ClearBackground(BLACK);

        for(size_t i = 3; i < (size_t)t.count; i++){
            float alpha = (float)i / t.count;
            DrawLineV(t.points[i-3], t.points[i], Fade(WHITE, alpha));
        }
        draw(&s, &t);

        EndDrawing();
    }

        CloseWindow();
        return 0;
    
}

void calcAcceleration(State* s, Params* p) {
    float d12 = fmaxf(dist(s->x1_pos, s->x2_pos, s->y1_pos, s->y2_pos), 2.0f * RADIUS);
    float d13 = fmaxf(dist(s->x1_pos, s->x3_pos, s->y1_pos, s->y3_pos), 2.0f * RADIUS);
    float d23 = fmaxf(dist(s->x2_pos, s->x3_pos, s->y2_pos, s->y3_pos), 2.0f * RADIUS);

    // Körper 1
    s->x1_acc = p->G*p->m2*(s->x2_pos-s->x1_pos)/(d12*d12*d12)
              + p->G*p->m3*(s->x3_pos-s->x1_pos)/(d13*d13*d13);
    s->y1_acc = p->G*p->m2*(s->y2_pos-s->y1_pos)/(d12*d12*d12)
              + p->G*p->m3*(s->y3_pos-s->y1_pos)/(d13*d13*d13);

    // Körper 2
    s->x2_acc = p->G*p->m1*(s->x1_pos-s->x2_pos)/(d12*d12*d12)
              + p->G*p->m3*(s->x3_pos-s->x2_pos)/(d23*d23*d23);
    s->y2_acc = p->G*p->m1*(s->y1_pos-s->y2_pos)/(d12*d12*d12)
              + p->G*p->m3*(s->y3_pos-s->y2_pos)/(d23*d23*d23);

    // Körper 3
    s->x3_acc = p->G*p->m1*(s->x1_pos-s->x3_pos)/(d13*d13*d13)
              + p->G*p->m2*(s->x2_pos-s->x3_pos)/(d23*d23*d23);
    s->y3_acc = p->G*p->m1*(s->y1_pos-s->y3_pos)/(d13*d13*d13)
              + p->G*p->m2*(s->y2_pos-s->y3_pos)/(d23*d23*d23);
}

void verletSpeed(State* s, Params* p) {
    // erste Hälfte
    calcAcceleration(s, p);
    
    // Körper 1
    s->x1_speed += s->x1_acc * p->dt / 2;
    s->y1_speed += s->y1_acc * p->dt / 2;
    // Körper 2
    s->x2_speed += s->x2_acc * p->dt / 2;
    s->y2_speed += s->y2_acc * p->dt / 2;
    // Körper 3
    s->x3_speed += s->x3_acc * p->dt / 2;
    s->y3_speed += s->y3_acc * p->dt / 2;

    // Positionsupdate
    // Körper 1
    s->x1_pos += s->x1_speed * p->dt;
    s->y1_pos += s->y1_speed * p->dt;
    // Körper 2
    s->x2_pos += s->x2_speed * p->dt;
    s->y2_pos += s->y2_speed * p->dt;
    // Körper 3
    s->x3_pos += s->x3_speed * p->dt;
    s->y3_pos += s->y3_speed * p->dt;

    // zweite Hälfte
    calcAcceleration(s, p);
    
    // Körper 1
    s->x1_speed += s->x1_acc * p->dt / 2;
    s->y1_speed += s->y1_acc * p->dt / 2;
    // Körper 2
    s->x2_speed += s->x2_acc * p->dt / 2;
    s->y2_speed += s->y2_acc * p->dt / 2;
    // Körper 3
    s->x3_speed += s->x3_acc * p->dt / 2;
    s->y3_speed += s->y3_acc * p->dt / 2;
}

float dist(float x1, float x2, float y1, float y2) {
    return sqrtf((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

void draw(const State* s, Trace* t) {
    // Körper 1
    DrawCircle((int)s->x1_pos, (int)s->y1_pos, RADIUS, WHITE);
    // Körper 2
    DrawCircle((int)s->x2_pos, (int)s->y2_pos, RADIUS, BLUE);
    // Körper 3
    DrawCircle((int)s->x3_pos, (int)s->y3_pos, RADIUS, RED);


    t->points[t->count++] = (Vector2){s->x1_pos, s->y1_pos};
    t->points[t->count++] = (Vector2){s->x2_pos, s->y2_pos};
    t->points[t->count++] = (Vector2){s->x3_pos, s->y3_pos};
}