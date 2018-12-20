#include <stdio.h>
#include <iostream>

#include <random>
#include <ctime>
#include <unistd.h>
#include <algorithm>
#include <math.h>
#include <vector>
#include "car.h"
//#include "texture.h"

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

car::car() {
    
    for (int i = 0; i < 3; i++) {
        speed.push_back(0.0);
    }
    
    for (int i = 0; i < 3; i++) {
        angle.push_back(0.0);
    }
    
    for (int i = 0; i < 3; i++) {
        carEye.push_back(EYE_START[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        carFocus.push_back(FOCUS_START[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        pos.push_back(POS_START[i]);
    }
    
    verts = {
        {-CAR_SIZE/2, -CAR_SIZE/2, -CAR_SIZE/2},
        {CAR_SIZE/2, -CAR_SIZE/2, -CAR_SIZE/2},
        {-CAR_SIZE/2, -CAR_SIZE/2, CAR_SIZE/2},
        {CAR_SIZE/2, -CAR_SIZE/2, CAR_SIZE/2},
        {-CAR_SIZE/2, CAR_SIZE/2, -CAR_SIZE/2},
        {CAR_SIZE/2, CAR_SIZE/2, -CAR_SIZE/2},
        {-CAR_SIZE/2, CAR_SIZE/2, CAR_SIZE/2},
        {CAR_SIZE/2, CAR_SIZE/2, CAR_SIZE/2}
    };
    
    indices = {
        {5,4,0,1},
        {7,5,1,3},
        {6,7,3,2},
        {4,6,2,0},
        {0,2,3,1},
        {5,7,6,4}
    };
    
    magnitude = sqrt(pow(speed[0], 2) + pow(speed[1], 2) + pow(speed[2], 2));
    
}

car::car(float boxSize) {            // skybox
    
    for (int i = 0; i < 3; i++) {
        speed.push_back(0.0);
    }
    
    for (int i = 0; i < 3; i++) {
        angle.push_back(0.0);
    }
    
    for (int i = 0; i < 3; i++) {
        carEye.push_back(EYE_START[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        carFocus.push_back(FOCUS_START[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        pos.push_back(POS_START[i]);
    }
    
    verts = {
        {-boxSize/2, -boxSize/2, -boxSize/2},
        {boxSize/2, -boxSize/2, -boxSize/2},
        {-boxSize/2, -boxSize/2, boxSize/2},
        {boxSize/2, -boxSize/2, boxSize/2},
        {-boxSize/2, boxSize/2, -boxSize/2},
        {boxSize/2, boxSize/2, -boxSize/2},
        {-boxSize/2, boxSize/2, boxSize/2},
        {boxSize/2, boxSize/2, boxSize/2}
    };
    
    indices = {
        /*
         {4,6,7,5},  //PosY
         {5,7,3,1},  //PosX
         {6,2,3,7},  //PosZ
         {0,4,5,1},  //NegZ
         {0,2,6,4},  //NegX
         {2,0,1,3}   //NegY
         */
        
        
        {1,0,4,5},
        {3,1,5,7},
        {2,3,7,6},
        {0,2,6,4},
        {1,3,2,0},
        {4,6,7,5}
        
        
        /*
         {5,4,0,1},
         {7,5,1,3},
         {6,7,3,2},
         {4,6,2,0},
         {0,2,3,1},
         {5,7,6,4}
         */
        
    };
    
    magnitude = sqrt(pow(speed[0], 2) + pow(speed[1], 2) + pow(speed[2], 2));
    
}

car::car(vector<float> inPos, vector<float> inAngle, float length, float width) {        // never called
    
    for (int i = 0; i < 3; i++) {        // set initial speed (0)
        speed.push_back(0.0);
    }
    
    for (int i = 0; i < 3; i++) {        // set initial angle
        angle.push_back(inAngle[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        carEye.push_back(EYE_START[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        carFocus.push_back(FOCUS_START[i]);
    }
    
    for (int i = 0; i < 3; i++) {        // set initial position
        pos.push_back(inPos[i]);
        lFrontWheelPos.push_back(inPos[i]);
        rFrontWheelPos.push_back(inPos[i]);
    }
    
    
    carWidth = width;
    carLength = length;
    
    wheelDistOffset = (carLength/6);
    
    for (int i = 0; i < 3; i++) {        //set left Front wheel position
        lFrontWheelPos.push_back((pos[i]) + ((carLength/2)- wheelDistOffset));
    }
    
}

void car::drawCar() {
    float temp[NUM_FACES][3];
    glColor3f(0.5, 0, 0.5);
    for(int ind = 0; ind < NUM_FACES; ind++){
        for (int i = 0; i < 3; i++) {           // making a temp array for using the face normals
            temp[ind][i] = faceNormals[ind][i];
        }
        
        glBegin(GL_POLYGON);
        glNormal3fv(temp[ind]);
        
        glTexCoord2f(0, 0);
        glVertex3f(verts[indices[ind][0]][0], verts[indices[ind][0]][1], verts[indices[ind][0]][2]);
        
        glTexCoord2f(0, 1);
        glVertex3f(verts[indices[ind][1]][0], verts[indices[ind][1]][1], verts[indices[ind][1]][2]);
        
        
        glTexCoord2f(1, 1);
        glVertex3f(verts[indices[ind][2]][0], verts[indices[ind][2]][1], verts[indices[ind][2]][2]);
        
        
        glTexCoord2f(1, 0);
        glVertex3f(verts[indices[ind][3]][0], verts[indices[ind][3]][1], verts[indices[ind][3]][2]);
        glEnd();
    }
}





