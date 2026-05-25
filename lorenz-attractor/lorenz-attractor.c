#include <math.h>
#include <raylib.h>
#include <stddef.h>

#define WIDTH 900
#define HEIGHT 900
#define FPS 60

#define SIGMA 10.0f
#define RHO 28.0f
#define BETA (8.0f / 3.0f) 
#define DT 0.001f

typedef struct State{
    float x, y, z; 
}State;

State f(const State* s);
State rk4(const State* s);
void initStates(State* states, size_t n);


int main(){
    InitWindow(WIDTH, HEIGHT, "Lorenz Attractor");
    SetTargetFPS(FPS);

    int drawingScale = 11;

    size_t n = 500;
    State states[n];
    initStates(states, n);

    while(!WindowShouldClose()){
        if(IsKeyPressed(KEY_SPACE)){
            initStates(states, n);
        }

        for(size_t i = 0; i < n; i++){
            for(size_t j = 0; j < 5; j++){
                states[i] = rk4(&states[i]);
            }
        }
        
        
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("Press SPACE to reset", 25, 25, 25, RAYWHITE);

        for(size_t i = 0; i < n; i++){
            float hue = (states[i].z / 50.0f) * 360.0f; 
            Color c = ColorFromHSV(hue, 1.0f, 1.0f);
            DrawCircle((WIDTH / 2) + states[i].x * drawingScale, (HEIGHT / 1.3) - states[i].z * drawingScale, 1.0, c);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void initStates(State* states, size_t n){
    for(size_t i = 0; i < n; i++){
        states[i].x = (float)GetRandomValue(0, WIDTH);
        states[i].y = (float)GetRandomValue(0, HEIGHT);
        states[i].z = 25.0f;  
    }
}

State f(const State* s){
    State ds;
    
    ds.x = SIGMA * (s->y - s->x);
    ds.y = s->x * (RHO - s->z) - s->y;
    ds.z = s->x * s->y - BETA * s->z;

    return ds; 
}

State rk4(const State* s){
    State k1, k2, k3, k4;
    State tmp;
    float h = DT;

    k1 = f(s);

    tmp.x = s->x + 0.5f * h * k1.x;
    tmp.y = s->y + 0.5f * h * k1.y;
    tmp.z = s->z + 0.5f * h * k1.z;
    k2 = f(&tmp);

    tmp.x = s->x + 0.5f * h * k2.x;
    tmp.y = s->y + 0.5f * h * k2.y;
    tmp.z = s->z + 0.5f * h * k2.z;
    k3 = f(&tmp);

    tmp.x = s->x + h * k3.x;
    tmp.y = s->y + h * k3.y;
    tmp.z = s->z + h * k3.z;
    k4 = f(&tmp);

    State result;
    
    result.x = s->x + (h / 6.0f) * (k1.x + 2 * k2.x + 2 * k3.x + k4.x);
    result.y = s->y + (h / 6.0f) * (k1.y + 2 * k2.y + 2 * k3.y + k4.y);
    result.z = s->z + (h / 6.0f) * (k1.z + 2 * k2.z + 2 * k3.z + k4.z);

    return result;
}


