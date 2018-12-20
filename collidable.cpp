#include <stdio.h>
#include <iostream>

#include <random>
#include <ctime>
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <string>
#include <math.h>
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

collidable::collidable() {
    
    for (int i = 0; i < 3; i++) {
        speed.push_back(0.0);
    }
    
    for (int i = 0; i < 3; i++) {
        angle.push_back(0.0);
    }
    
    for (int i = 0; i < 3; i++) {
        pos.push_back(0.0);
    }
    name = "name hobo";
    
}

collidable::collidable(vector<float> inPos, vector<float> inAngle, int typeID) {
    
    for (int i = 0; i < 3; i++) {        // set initial speed (0)
        speed.push_back(0.0);
    }
    
    for (int i = 0; i < 3; i++) {        // set initial angle
        angle.push_back(inAngle[i]);
    }
    
    for (int i = 0; i < 3; i++) {        // set initial position
        pos.push_back(inPos[i]);
    }
    type = typeID;
    
}

// Class Methods

void collidable::setName (string myName) {
    name = myName;
}

float collidable::vectorMagnitude(vector<float> v) {
    return sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
}

float collidable::vectorMagnitudeArray(float v[]) {
    return sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
}

vector<float> collidable::unitVector(vector<float> Vec){
    float magnitude = vectorMagnitude(Vec);
    vector<float> temp = {0,0,0};
    temp[0] = Vec[0] / magnitude;
    temp[1] = Vec[1] / magnitude;
    temp[2] = Vec[2] / magnitude;
    
    return temp;
}

vector<float> collidable::vectorSubtraction(vector<float> a, vector<float> b){
    vector<float> temp = {0,0,0};
    temp[0] = a[0] - b[0];
    temp[1] = a[1] - b[1];
    temp[2] = a[2] - b[2];
    
    return temp;
}

vector<float> collidable::crossProduct(vector<float> v1, vector<float> v2) {
    vector<float> v3;
    v3.push_back(v1[1]*v2[2] - v1[2]*v2[1]);
    v3.push_back(v1[2]*v2[0] - v1[0]*v2[2]);
    v3.push_back(v1[0]*v2[1] - v1[1]*v2[0]);
    return v3;
}

void collidable::setBoundBox(vector<vector<float>> verts) {
    
    float minX = verts[0][0];
    float maxX = verts[0][0];
    float minY = verts[0][1];
    float maxY = verts[0][1];
    float minZ = verts[0][2];
    float maxZ = verts[0][2];
    
    for (int i = 1; i < verts.size(); i++) {
        
        if (verts[i][0] < minX) {
            minX = verts[i][0];
        }
        else if (verts[i][0] > maxX) {
            maxX = verts[i][0];
        }
        
        if (verts[i][1] < minY) {
            minY = verts[i][1];
        }
        else if (verts[i][1] > maxY) {
            maxY = verts[i][1];
        }
        
        if (verts[i][2] < minZ) {
            minZ = verts[i][2];
        }
        else if (verts[i][2] > maxZ) {
            maxZ = verts[i][2];
        }
        
    }
    
    while (boundBox.size() != 0) {
        boundBox.pop_back();
    }
    
    boundBox.push_back(minX + pos[0]);
    boundBox.push_back(maxX + pos[0]);
    boundBox.push_back(minY + pos[1]);
    boundBox.push_back(maxY + pos[1]);
    boundBox.push_back(minZ + pos[2]);
    boundBox.push_back(maxZ + pos[2]);
}

void collidable::drawBoundBox() {
    glColor3f(0, 1, 0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(boundBox[0], boundBox[2], boundBox[4]);
    glVertex3f(boundBox[1], boundBox[2], boundBox[4]);
    glVertex3f(boundBox[1], boundBox[2], boundBox[5]);
    glVertex3f(boundBox[0], boundBox[2], boundBox[5]);
    glVertex3f(boundBox[0], boundBox[2], boundBox[4]);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(boundBox[0], boundBox[3], boundBox[4]);
    glVertex3f(boundBox[1], boundBox[3], boundBox[4]);
    glVertex3f(boundBox[1], boundBox[3], boundBox[5]);
    glVertex3f(boundBox[0], boundBox[3], boundBox[5]);
    glVertex3f(boundBox[0], boundBox[3], boundBox[4]);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(boundBox[0], boundBox[2], boundBox[4]);
    glVertex3f(boundBox[0], boundBox[3], boundBox[4]);
    glVertex3f(boundBox[1], boundBox[2], boundBox[4]);
    glVertex3f(boundBox[1], boundBox[3], boundBox[4]);
    glVertex3f(boundBox[1], boundBox[2], boundBox[5]);
    glVertex3f(boundBox[1], boundBox[3], boundBox[5]);
    glVertex3f(boundBox[0], boundBox[2], boundBox[5]);
    glVertex3f(boundBox[0], boundBox[3], boundBox[5]);
    glEnd();
}

void collidable::calcFaceNorms() {
    vector<float> tempA = {0,0,0};
    vector<float> tempB = {0,0,0};
    
    for (int i = 0; i < indices.size(); i++) {
        
        tempA = vectorSubtraction(verts[indices[i][1]], verts[indices[i][0]]);
        /*for (int i = 0; i < 3; i++) {           // making a temp array for using the face normals
         printf("tempB[%d] = %f ",  i, tempB[i]);
         }*/
        tempB = vectorSubtraction(verts[indices[i][3]], verts[indices[i][0]]);
        /*for (int i = 0; i < 3; i++) {           // making a temp array for using the face normals
         printf("tempA[%d] = %f ",  i, tempA[i]);
         }*/
        faceNormals.push_back(crossProduct(tempA,tempB));
        
    }
    
}

bool collidable::collide(collidable other) {
    if (boundBox[0] > other.boundBox[1] or boundBox[1] < other.boundBox[0]){
        return false;
    }
    if (boundBox[2] > other.boundBox[3] or boundBox[3] < other.boundBox[2]){
        return false;
    }
    if (boundBox[4] > other.boundBox[5] or boundBox[5] < other.boundBox[4]){
        return false;
    }
    return true;
}

void collidable::setVerts(vector<vector<float>> vs){
    verts.clear();
    vector<float> temp;
    for (int i = 0; i < vs.size(); i++){
        for (int j = 0; j < vs[i].size(); j++){
            temp.push_back(vs[i][j]);
        }
        verts.push_back(temp);
    }
}

void collidable::drawBuilding(){
    float temp[indices.size()][3];
    glColor3f(1, 1, 0);
    for(int ind = 0; ind < indices.size(); ind++){
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
