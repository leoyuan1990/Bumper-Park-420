#ifndef collidable_h
#define collidable_h
#include <stdio.h>

#include <math.h>
#include <vector>
#include <string>

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

class collidable {
public:
    collidable();
    collidable(vector<float> inPos, vector<float> inAngle, int typeID);
    
    vector<float> pos;
    vector<float> speed;
    vector<float> angle;
    vector<vector<float>> verts;
    vector<vector<int>> indices;
    vector<vector<float>> faceNormals;
    vector<float> boundBox;            // {minX, maxX, minY, maxY, minZ, maxZ}
    
    int type = 0;    // the type of the collidable (0 == nothing, 1 == NPC, 2 == powerup, >= 3 is different boosts)
    string name;                // name of the collidable
    
    float magnitude = 0;        // the total magnitude of the speed vector
    int NUM_FACES = 6;
    int NUM_VERTS = 8;
    
    // ------- class methods -------
    void setName(string myName);
    float vectorMagnitude(vector<float> v);
    float vectorMagnitudeArray(float v[]);
    vector<float> unitVector(vector<float> v);
    vector<float> vectorSubtraction(vector<float> a, vector<float> b);
    vector<float> crossProduct(vector<float> v1, vector<float> v2);
    void setBoundBox(vector<vector<float>> vertices);
    void drawBoundBox();
    void calcFaceNorms();
    bool collide(collidable other);
    void setVerts(vector<vector<float>> vs);
    void drawBuilding();
    
    //float dotProduct(vector<float> v1, vector<float> v2);
    //vector<float> crossProduct(vector<float> v1, vector<float> v2);
    //vector<float> project(vector<float> axis);
    
};
#endif
