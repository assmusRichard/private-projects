#include <raylib.h>
#include <math.h>

#define WIDTH 1200
#define HEIGHT 900
#define FPS 120

typedef struct State{
    float theta1, theta2;
    float omega1, omega2;
}State;

typedef struct Params{
    float m1, m2;   // Masse 1, 2
    float l1, l2;   // Länge Stab 1, Stab 2
    float g;        // Erdbeschleunigung
    float dt;       // Zeitschritt 
} Params;

// Vektorfunktion der Differentialgleichung
State f(const State* s, const Params* p);

// Teilfunktionen: Omega (Winkelbeschleunigung) berechnen
float F1(float th1, float th2, float o1, float o2, const Params* p);
float F2(float th1, float th2, float o1, float o2, const Params* p);

// RK4 kombiniert beides
State rk4(const State* s, const Params* p);

// Pendel zeichnen
void drawPendulum(const State* s, const Params* p, const Vector2* origin);

int main(){
    //Startzustand des Pendels (beide Stäbe)
    State s1 = {.theta1 = 0.3f, .theta2 = 2.8, .omega1 = 0, .omega2 = 0};
    Params p = {.m1 = 20, .m2 = 20, .l1 = 120, .l2 = 120, .g = 981.0f, .dt = 0.001f};

    // Fester Punkt des Pendels
    Vector2 origin = {WIDTH / 2, 100};

    InitWindow(WIDTH, HEIGHT, "Double Pendulum RK4");
    SetTargetFPS(FPS);

    while(!WindowShouldClose()){
        for(int i = 0; i < 10; i++){
            s1 = rk4(&s1, &p);
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        drawPendulum(&s1, &p, &origin);
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void drawPendulum(const State* s, const Params* p, const Vector2* origin){
    float x1_pos, y1_pos;   // Stab 1
    float x2_pos, y2_pos;   // Stab 2

    // Stab 1 Poisitionsupdate
    x1_pos = origin->x + p->l1 * sinf(s->theta1);
    y1_pos = origin->y + p->l1 * cosf(s->theta1);

    // Stab 2 Positionsupdate
    x2_pos = x1_pos + p->l2 * sinf(s->theta2);
    y2_pos = y1_pos + p->l2 * cosf(s->theta2);

    // Stab 1 zeichnen
    DrawLineEx(*origin, (Vector2){x1_pos, y1_pos}, 2.0f, WHITE);

    // Stab 2 zeichnen
    DrawLineEx((Vector2){x1_pos, y1_pos}, (Vector2){x2_pos, y2_pos}, 2.0f, WHITE);

    // Masse 1 zeichnen
    DrawCircle((int)x1_pos, y1_pos, 15, RED);

    // Masse 2 zeichnen
    DrawCircle((int)x2_pos, (int)y2_pos, 15, RED);
}

State rk4(const State* s, const Params* p){
    State k1, k2, k3, k4;
    State tmp;

    float h = p->dt;

    k1 = f(s, p);
    tmp.theta1 = s->theta1 + 0.5 * h * k1.theta1;
    tmp.theta2 = s->theta2 + 0.5 * h * k1.theta2;
    tmp.omega1 = s->omega1 + 0.5 * h * k1.omega1;
    tmp.omega2 = s->omega2 + 0.5 * h * k1.omega2;

    k2 = f(&tmp, p);
    tmp.theta1 = s->theta1 + 0.5 * h * k2.theta1;
    tmp.theta2 = s->theta2 + 0.5 * h * k2.theta2;
    tmp.omega1 = s->omega1 + 0.5 * h * k2.omega1;
    tmp.omega2 = s->omega2 + 0.5 * h * k2.omega2;

    k3 = f(&tmp, p);
    tmp.theta1 = s->theta1 + h * k3.theta1;
    tmp.theta2 = s->theta2 + h * k3.theta2;
    tmp.omega1 = s->omega1 + h * k3.omega1;
    tmp.omega2 = s->omega2 + h * k3.omega2;

    k4 = f(&tmp, p);
    
    State result;
    result.theta1 = s->theta1 + (h / 6) * (k1.theta1 + 2 * k2.theta1 + 2 * k3.theta1 + k4.theta1);
    result.theta2 = s->theta2 + (h / 6) * (k1.theta2 + 2 * k2.theta2 + 2 * k3.theta2 + k4.theta2);
    result.omega1 = s->omega1 + (h / 6) * (k1.omega1 + 2 * k2.omega1 + 2 * k3.omega1 + k4.omega1);
    result.omega2 = s->omega2 + (h / 6) * (k1.omega2 + 2 * k2.omega2 + 2 * k3.omega2 + k4.omega2);

    // result.omega1 *= 0.9999f;
    // result.omega2 *= 0.9999f;

    return result;
}

State f(const State* s, const Params* p){
    State ds;
    ds.theta1 = s->omega1;
    ds.theta2 = s->omega2;

    ds.omega1 = F1(s->theta1, s->theta2, s->omega1, s->omega2, p);
    ds.omega2 = F2(s->theta1, s->theta2, s->omega1, s->omega2, p);

    return ds;
}

float F1(float th1, float th2, float o1, float o2, const Params* p){
    float g = p->g;
    float m1 = p->m1;
    float m2 = p->m2;
    float l1 = p->l1;
    float l2 = p->l2;

    float numerator;
    float denominator;

    numerator = -g*(m1+m2)*sinf(th1)-m2*g*sinf(th1-2*th2)-2*sinf(th1-th2)*(m2*l2*o2*o2+(m1+m2)*l1*o1*o1*cosf(th1-th2));
    denominator = l1*(2*m1+m2-m2*cosf(2*(th1-th2)));

    float eps = 1e-6f;
    denominator = (fabsf(denominator) < eps) ? copysignf(eps, denominator) : denominator;

    return numerator / denominator;
}

float F2(float th1, float th2, float o1, float o2, const Params* p){
    float g = p->g;
    float m1 = p->m1;
    float m2 = p->m2;
    float l1 = p->l1;
    float l2 = p->l2;

    float numerator;
    float denominator;

    numerator = 2*sinf(th1-th2)*((m1+m2)*l1*o1*o1+g*(m1+m2)*cosf(th1)+m2*l2*o2*o2*cosf(th1-th2));
    denominator = l2*(2*m1+m2-m2*cosf(2*(th1-th2)));

    float eps = 1e-6f;
    denominator = (fabsf(denominator) < eps) ? copysignf(eps, denominator) : denominator;

    return numerator / denominator;
}


