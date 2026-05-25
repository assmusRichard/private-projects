
#include <math.h>
#include <raylib.h>
#include <stdlib.h>

#define WIDTH GetScreenWidth()
#define HEIGHT GetScreenHeight()
#define FPS 60
#define RADIUS 15.0f

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
    Vector2* points;
    Color* colors;
    int head;
    int count;
    int max;
}Trace;

void verletSpeed(State* s, Params* p);
void calcAcceleration(State* s, Params* p);
float dist(float x1, float x2, float y1, float y2);
void draw(const State* s, Trace* t);
State initBodies();

int main() {
    InitWindow(WIDTH, HEIGHT, "Three Body Problem");
    SetTargetFPS(FPS);

    Params p = { 
        .G = 15.0f,
        .m1 = 20, .m2 = 21, .m3 = 22,
        .dt = 0.0099f
    };

    Trace t = {.head = 0, .count = 0, .max = 3 * 10000};
    t.points = malloc(sizeof(Vector2) * t.max);
    t.colors = malloc(sizeof(Color) * t.max);

    State s = initBodies();

    while(!WindowShouldClose()) {
        if(IsKeyPressed(KEY_SPACE)){
            s = initBodies();
            t.head = 0;
            t.count = 0;
        } 

        for(int i = 0; i < 200; i++){
            verletSpeed(&s, &p);
        }

        BeginDrawing();

        ClearBackground(BLACK);

        DrawText("Press SPACE to reset", 25, 25, 25, RAYWHITE);

        for(size_t i = 3; i < (size_t)t.count; i++){
            float alpha = (float)i / t.count;

            int idxA = (t.head - t.count - 3 + i + t.max) % t.max;
            int idxB = (t.head - t.count + i + t.max) % t.max;

            DrawLineV(t.points[idxA], t.points[idxB], Fade(t.colors[idxA], alpha));
        }
        draw(&s, &t);

        EndDrawing();
    }

        CloseWindow();
        return 0;
    
}

State initBodies(){
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

    return s;
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


    t->points[t->head] = (Vector2){s->x1_pos, s->y1_pos};
    t->colors[t->head] = WHITE;
    t->head = (t->head + 1) % t->max;

    t->points[t->head] = (Vector2){s->x2_pos, s->y2_pos};
    t->colors[t->head] = BLUE;
    t->head = (t->head + 1) % t->max;

    t->points[t->head] = (Vector2){s->x3_pos, s->y3_pos};
    t->colors[t->head] = RED;
    t->head = (t->head + 1) % t->max;

    if(t->count < t->max) t->count++;
}