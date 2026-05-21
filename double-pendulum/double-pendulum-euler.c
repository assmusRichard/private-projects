#include <raylib.h>
#include <math.h>
#include <stdio.h>

#define WIDTH 1200
#define HEIGHT 900
#define FPS 120

const int samplingSteps = 10;

typedef struct PendulumParams{
    float m1;           // Mass 1
    float m2;           // Mass 2
    float l1;           // Length 1
    float l2;           // Length 2
}PendulumParams;

typedef struct PhysicParams{
    float g;            // Gravitational constant 
    float theta1;       // Angle 1
    float theta2;       // Angle 2
    float theta_dot1;   // Angle velocity 1
    float theta_dot2;   // Angle velocity 2
    float dt;           // Timestep 
}PhysicParams;

float calcTheta_ddot1(PendulumParams* penP, PhysicParams* phyP);
float calcTheta_ddot2(PendulumParams* penP, PhysicParams* phyP);
void updateTheta_dot1(PendulumParams* penP, PhysicParams* phyP);
void updateTheta_dot2(PendulumParams* penP, PhysicParams* phyP);
void updateTheta1(PhysicParams* phyP);
void updateTheta2(PhysicParams* phyP);
void drawPendulum(PendulumParams* penP, PhysicParams* phyP);    

Vector2 origin = {WIDTH / 2, 100};    // Fix position for rod 1

int main(){
    InitWindow(WIDTH, HEIGHT, "Double Pendulum");
    SetTargetFPS(FPS);

    PendulumParams penP = {.m1 = 20.0f, .m2 = 20.0f, .l1 = 120.0f, .l2 = 120.0f};
    PhysicParams phyP = {.g = 981.0f, .theta1 = 0.4f, .theta2 = 2.4f, .theta_dot1 = 0.0f, .theta_dot2 = 0.0f, .dt = 0.001};

    while(!WindowShouldClose()){
        for(int i = 0; i < samplingSteps; i++){
            float theta_ddot1 = calcTheta_ddot1(&penP, &phyP);
            float theta_ddot2 = calcTheta_ddot2(&penP, &phyP);

            phyP.theta_dot1 += theta_ddot1 * phyP.dt;
            phyP.theta_dot2 += theta_ddot2 * phyP.dt;

            phyP.theta1 += phyP.theta_dot1 * phyP.dt;
            phyP.theta2 += phyP.theta_dot2 * phyP.dt;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        drawPendulum(&penP, &phyP);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

float calcTheta_ddot1(PendulumParams* penP, PhysicParams* phyP){
    float numerator;
    float denominator;

    float theta1 = phyP->theta1;
    float theta2 = phyP->theta2;
    float theta_dot1 = phyP->theta_dot1;
    float theta_dot2 = phyP->theta_dot2;
    float g = phyP->g;
    float m1 = penP->m1;
    float m2 = penP->m2;
    float l1 = penP->l1;
    float l2 = penP->l2;

    numerator = -g*(m1+m2)*sinf(theta1)-m2*g*sinf(theta1-2*theta2)-2*sinf(theta1-theta2)*(m2*l2*theta_dot2*theta_dot2+(m1+m2)*l1*theta_dot1*theta_dot1*cosf(theta1-theta2));
    denominator = l1*(2*m1+m2-m2*cosf(2*theta1-2*theta2));

    if (fabsf(denominator) < 0.001f) denominator = copysignf(0.001f, denominator);

    return numerator / denominator;
}

float calcTheta_ddot2(PendulumParams* penP, PhysicParams* phyP){
    float numerator;
    float denominator;

    float theta1 = phyP->theta1;
    float theta2 = phyP->theta2;
    float theta_dot1 = phyP->theta_dot1;
    float theta_dot2 = phyP->theta_dot2;
    float g = phyP->g;
    float m1 = penP->m1;
    float m2 = penP->m2;
    float l1 = penP->l1;
    float l2 = penP->l2;

    numerator = 2*sinf(theta1-theta2)*((m1+m2)*l1*theta_dot1*theta_dot1+g*(m1+m2)*cosf(theta1)+m2*l2*theta_dot2*theta_dot2*cosf(theta1-theta2));
    denominator = l2*(2*m1+m2-m2*cosf(2*theta1-2*theta2));

    if (fabsf(denominator) < 0.001f) denominator = copysignf(0.001f, denominator);

    return numerator / denominator;
}

/*
void updateTheta_dot1(PendulumParams* penP, PhysicParams* phyP){
    float theta_ddot1 = calcTheta_ddot1(penP, phyP);
    phyP->theta_dot1 = phyP->theta_dot1+theta_ddot1*phyP->dt;
    // theta_dot1 *= 0.9999f;
}

void updateTheta_dot2(PendulumParams* penP, PhysicParams* phyP){
    float theta_ddot2 = calcTheta_ddot2(penP, phyP);
    phyP->theta_dot2 = phyP->theta_dot2+theta_ddot2*phyP->dt;
    // theta_dot2 *= 0.9999f;
}

void updateTheta1(PhysicParams* phyP){
    phyP->theta1 = phyP->theta1+phyP->theta_dot1*phyP->dt;
}

void updateTheta2(PhysicParams* phyP){
    phyP->theta2 = phyP->theta2+phyP->theta_dot2*phyP->dt;
}
*/

void drawPendulum(PendulumParams* penP, PhysicParams* phyP){
    // Rod 1 position update
    float x1_pos = origin.x+penP->l1*sinf(phyP->theta1);
    float y1_pos = origin.y+penP->l1*cosf(phyP->theta1);

    // Rod 2 position update
    float x2_pos = x1_pos+penP->l2*sinf(phyP->theta2);
    float y2_pos = y1_pos+penP->l2*cosf(phyP->theta2);

    // Rod 1
    DrawLineEx(origin, (Vector2){x1_pos, y1_pos}, 2.0f, WHITE);    
    // Rod 2         
    DrawLineEx((Vector2){x1_pos, y1_pos}, (Vector2){x2_pos, y2_pos}, 2.0f, WHITE);  

    // Mass 1
    DrawCircle((int)x1_pos, (int)y1_pos, (int)penP->m1, RED);    
    // Mass 2                      
    DrawCircle((int)x2_pos, (int)y2_pos, (int)penP->m2, YELLOW);                          
}