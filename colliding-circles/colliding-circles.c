#include <stdio.h>
#include <raylib.h>
#include <math.h>

#define WIDTH 900
#define HEIGHT 600
#define FPS 100

#define MIN_MASS 5
#define MAX_MASS 30
#define SPEED 100
#define NUM_CIRCLES 10

#define E 1.0f //Restitutionskoeffizient (Maß für die Elasizität eines Stoßes) 0 <= E <= 1
#define TIME_STEP 0.02f


typedef struct Circle{
    float x_pos, y_pos;
    float x_speed, y_speed;
    float mass; 
    int radius;
}Circle;

Circle circles[NUM_CIRCLES];

void initCircles(){
    for(int i = 0; i < NUM_CIRCLES; i++){
        circles[i].mass = GetRandomValue(MIN_MASS, MAX_MASS);
        circles[i].radius = circles[i].mass;
        circles[i].x_pos = GetRandomValue(circles[i].radius, WIDTH - circles[i].radius);
        circles[i].y_pos = GetRandomValue(circles[i].radius, HEIGHT - circles[i].radius);
        circles[i].x_speed = GetRandomValue(-SPEED, SPEED);
        circles[i].y_speed = GetRandomValue(-SPEED, SPEED);
    }
}

void drawCircles(){
    for(int i = 0; i < NUM_CIRCLES; i++){
        DrawCircle( circles[i].x_pos, 
                    circles[i].y_pos, 
                    circles[i].radius, 
                    RAYWHITE);
    }
}

void updateCircles(){
    for(int i = 0; i < NUM_CIRCLES; i++){
        Circle *c = &circles[i];

        c->x_pos += c->x_speed * TIME_STEP;
        c->y_pos += c->y_speed * TIME_STEP;

        if(c->x_pos < c->radius || c->x_pos > WIDTH - c->radius){
            c->x_speed *= -1;
        }

        if(c->y_pos < c->radius || c->y_pos > HEIGHT - c->radius){
            c->y_speed *= -1;
        }    
    }
}

bool checkIfCollision(Circle *c1, Circle *c2){
    float dist = sqrtf(pow(c1->x_pos - c2->x_pos, 2) + pow(c1->y_pos - c2->y_pos, 2));

    //Vektor von c1 nach c2 und diese normalisieren
    float dx = c1->x_pos - c2->x_pos;
    float dy = c1->y_pos - c2->y_pos;

    if(dist == 0) return false;

    float nx = dx / dist; 
    float ny = dy / dist;
    
    //relative Geschwindigkeit von c1 zu c2
    float x_speedRelative = c1->x_speed - c2->x_speed;
    float y_speedRelative = c1->y_speed - c2->y_speed; 

    return(dist <= c1->radius + c2->radius && (x_speedRelative * nx + y_speedRelative * ny) < 0);
}

void handleCollision(Circle *c1, Circle *c2){
    float dist = sqrtf(pow(c1->x_pos - c2->x_pos, 2) + pow(c1->y_pos - c2->y_pos, 2));

    float dx = c1->x_pos - c2->x_pos;
    float dy = c1->y_pos - c2->y_pos;

    float nx = dx / dist; 
    float ny = dy / dist;

    //Impuls J berechnen
    float x_speedRelative = c1->x_speed - c2->x_speed;
    float y_speedRelative = c1->y_speed - c2->y_speed; 

    float impuls = ((1+E) * (x_speedRelative * nx + y_speedRelative * ny)) / ((1 / c1->mass) + (1 / c2->mass));

    //neue Geschwindigkeiten berechnen
    float new_vx1 = c1->x_speed - (impuls / c1->mass) * nx;
    float new_vy1 = c1->y_speed - (impuls / c1->mass) * ny;

    float new_vx2 = c2->x_speed + (impuls / c2->mass) * nx;
    float new_vy2 = c2->y_speed + (impuls / c2->mass) * ny;

    //Überlappung entfernen
    float overlap = c1->radius + c2->radius - dist;

    c1->x_pos -= (overlap / 2) * nx * (c2->mass / (c1->mass + c2->mass));
    c1->y_pos -= (overlap / 2) * ny * (c2->mass / (c1->mass + c2->mass));

    c2->x_pos += (overlap / 2) * nx * (c2->mass / (c1->mass + c2->mass));
    c2->y_pos += (overlap / 2) * ny * (c2->mass / (c1->mass + c2->mass));

    //Geschwindigkeiten aktualisieren
    c1->x_speed = new_vx1;
    c1->y_speed = new_vy1;

    c2->x_speed = new_vx2;
    c2->y_speed = new_vy2;
}

void checkAndHandleCollision(){
    for(int i = 0; i < NUM_CIRCLES; i++){
        for(int j = i + 1; j < NUM_CIRCLES; j++){
            if(checkIfCollision(&circles[i], &circles[j])){
                handleCollision(&circles[i], &circles[j]);
            }
        }
    }
}


int main(){
    InitWindow(WIDTH, HEIGHT, "Simulation colliding Circles");
    initCircles();

    SetTargetFPS(FPS);

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);

        for(int i = 0; i < 5; i++){
            updateCircles();
            checkAndHandleCollision();
        }

        drawCircles();
        

        EndDrawing();
    }

    CloseWindow();





    return 0;


}