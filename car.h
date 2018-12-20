#ifndef car_h
#define car_h
#include <stdio.h>
#include <math.h>
#include <vector>
#include "collidable.h"

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

using namespace std;

class car : public collidable {
public:
    car();
    car(float boxSize);
    car(vector<float> inPos, vector<float> inAngle, float length, float width);
    
    const float CAR_SIZE = 1;
    const float FLOAT_COEFF = 1;
    
    float turnAngle = 0;
    
    const float TURN_ANGLE_MAX = 30;
    
    vector<float> rFrontWheelPos;
    vector<float> lFrontWheelPos;
    
    float wheelDistOffset = 0;  // ------- note, ALWAYS initialize these singular variables so that weird shit don't happen! -------
    float carLength = 0;
    float carWidth = 0;
    
    vector<float> carEye;               // initialized to EYE_START
    vector<float> carFocus;             // initialized to FOCUS_START
    vector<float> cubeCamOffset = {5, 2, 5};    // offset of camera from cube
    const float OBJ_START_HEIGHT = CAR_SIZE * (1 + FLOAT_COEFF)/2;
    const vector<float> EYE_START = {-5.0, 2, -5.0};     // -5, 2, -5 and 8, 0, 8 for FOCUS_START seem to be a good "third person" view
    const vector<float> FOCUS_START = {8.0, 0.0, 8.0};
    const vector<float> POS_START = {EYE_START[0] + cubeCamOffset[0], OBJ_START_HEIGHT, EYE_START[2] + cubeCamOffset[2]};
    
    
    // ------- class methods -------
    void drawCar();
    
};
#endif
