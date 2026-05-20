#include <raylib.h>
#include <math.h>
#include <stdio.h>

#define WIDTH 1200
#define HEIGHT 900
#define FPS 120

int samplingSteps = 15;
float m1 = 30.0f;                  // Mass 1
float m2 = 40.0f;                  // Mass 2
float l1 = 200.0f;                  // Length 1
float l2 = 200.0f;                 // Length 2
float g = 9.81f;                     // Gravitational constant 
float theta1 = 0.1f;
float theta2 = 3.0f;
float theta_dot1 = 0.0f;            // Angle velocity 1
float theta_dot2 = 0.0f;            // Angle velocity 2
float theta_ddot1;                  // Angle acceleration 1
float theta_ddot2;                  // Angle acceleration 2
float dt = 0.008;                  // Time step
float x1_pos, y1_pos;
float x2_pos, y2_pos;

float calcTheta_ddot1();
float calcTheta_ddot2();
void updateTheta_dot1();
void updateTheta_dot2();
void updateTheta1();
void updateTheta2();
void drawPendulum();    

Vector2 origin = {WIDTH / 2, 100};    // Fix position for rod 1

int main(){
    InitWindow(WIDTH, HEIGHT, "Double Pendulum");
    SetTargetFPS(FPS);

    while(!WindowShouldClose()){
        for(int i = 0; i < samplingSteps; i++){
            updateTheta_dot1();
            updateTheta_dot2();
            updateTheta1();
            updateTheta2();
        }

        BeginDrawing();
        ClearBackground(BLACK);
        drawPendulum();
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

float calcTheta_ddot1(){
    float numerator;
    float denominator;

    numerator = -g*(m1+m2)*sinf(theta1)-m2*g*sinf(theta1-2*theta2)-2*sinf(theta1-theta2)*(m2*l2*theta_dot1*theta_dot1+(m1+m2)*l1*theta_dot1*theta_dot1*cosf(theta1-theta2));
    denominator = l1*(2*m1+m2-m2*cosf(2*theta1-2*theta2));

    if (fabsf(denominator) < 0.001f) denominator = 0.001f;

    return numerator / denominator;
}

float calcTheta_ddot2(){
    float numerator;
    float denominator;

    numerator = 2*sinf(theta1-theta2)*((m1+m2)*l1*theta_dot1*theta_dot1+g*(m1+m2)*cosf(theta1)+m2*l2*theta_dot2*theta_dot2*cosf(theta1-theta2));
    denominator = l2*(2*m1+m2-m2*cosf(2*theta1-2*theta2));

    if (fabsf(denominator) < 0.001f) denominator = 0.001f;

    return numerator / denominator;
}

void updateTheta_dot1(){
    theta_ddot1 = calcTheta_ddot1();
    theta_dot1 = theta_dot1+theta_ddot1*dt;
    theta_dot1 *= 0.9999f;
}

void updateTheta_dot2(){
    theta_ddot2 = calcTheta_ddot2();
    theta_dot2 = theta_dot2+theta_ddot2*dt;
    theta_dot2 *= 0.9999f;
}

void updateTheta1(){
    theta1 = theta1+theta_dot1*dt;
}

void updateTheta2(){
    theta2 = theta2+theta_dot2*dt;
}


void drawPendulum(){
    // Rod 1 position update
    x1_pos = origin.x+l1*sinf(theta1);
    y1_pos = origin.y+l1*cosf(theta1);

    // Rod 2 position update
    x2_pos = x1_pos+l2*sinf(theta2);
    y2_pos = y1_pos+l2*cosf(theta2);

    // Rod 1
    DrawLineEx(origin, (Vector2){x1_pos, y1_pos}, 2.0f, WHITE);    
    // Rod 2         
    DrawLineEx((Vector2){x1_pos, y1_pos}, (Vector2){x2_pos, y2_pos}, 2.0, WHITE);  

    // Mass 1
    DrawCircle((int)x1_pos, (int)y1_pos, (int)m1/2, RED);    
    // Mass 2                      
    DrawCircle((int)x2_pos, (int)y2_pos, (int)m2/2, YELLOW);                          
}