/* Code from our assignments have been recycled in the creation of this project
 
 BUMPER PARK 420
 
 Evan Reaume     reaumee     001450061
 Danny Stewart   stewardl    001224480
 Fangzhou Yuan   yuanf2      000861962
 
 */

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include <random>
#include <ctime>
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <string>
#include "collidable.h"
#include "car.h"
#include "texture.h"

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

struct ray{
    vector<float> end;
    vector<float> start;
    vector<float> dir;
    float length;
} picker;

using namespace std;

// window size and initial position (adjust to your liking)
const int INITIAL_X_POS = 300;
const int INITIAL_Y_POS = 0;
const int MAX_X = 800;
const int MAX_Y = 800;

// HUD
const float HUD_COLOR[3] = {0, 1, 0};
const float HUD_SPACING = MAX_X * 0.125;          // how much to indent HUD values (for example, the actual speed after "SPEED: ")
const float HUD_SPEED_X_POS = MAX_X * 0.07;
const float HUD_SPEED_Y_POS = MAX_Y * 0.92;
const float HUD_ANGLE_X_POS = MAX_X * 0.07;
const float HUD_ANGLE_Y_POS = MAX_Y * 0.86;
const float HUD_OBJNAME_X_POS = MAX_X * 0.6;
const float HUD_OBJNAME_Y_POS = MAX_Y * 0.92;
const float HUD_SCORE_X_POS = MAX_X * 0.6;
const float HUD_SCORE_Y_POS = MAX_Y * 0.86;

// map
const int MAP_SIZE = 200;               // keep this as an int
const float CHECKER_SIZE = 0.5;
const int NUM_BUMPS_SCALING = 300;      // scaling factor for maintaing bump density for any map size (higher is flatter map)
const int NUM_BUMPS = (int) (pow(MAP_SIZE * CHECKER_SIZE, 2)/NUM_BUMPS_SCALING);          // number of bumps in the map
const float BUMP_RADIUS = 20;           // radius of bumps
const float BUMP_HEIGHT = 5;            // height of bumps (for valleys it's negative)
float map [MAP_SIZE][MAP_SIZE][3];      // n x n matrix of 3d points
float faceNorms [MAP_SIZE - 1][MAP_SIZE - 1][3];    // matrix of face normals for each checker
float vertexNorms [MAP_SIZE][MAP_SIZE][3];          // matrix of vertex normals for each vertice of the checkerboard
vector<vector<float>> heights;          // height field
bool insideMap = true;
bool onMap = false;
const float ON_MAP_LIMIT = 0.01;        // car must be no more than this distance above the map to be considered on the map

//Material for mesh
float matAmb0[] = {0.05375f, 0.05f, 0.06625f, 0.82f};
float matDiff0[] = {0.18275f, 0.17f, 0.22525f, 0.82f};
float matSpec0[] = {0.332741f, 0.328634f, 0.346435f, 0.82f};
float shiny0 = 38.4f ;

//Material properties for cube
float matAmb1[] = {0.5, 0.5, 0.5, 0.1};
float matDiff1[] = {0.5, 0.5, 0.5, 0.1};
float matSpec1[] = {0.5, 0.5, 0.5, 0.1};
float shiny1 = 10.0;

//Material properties for skybox
float matAmb2[] = {0.5, 0.5, 0.5, 0.1};
float matDiff2[] = {0.5, 0.5, 0.5, 0.1};
float matSpec2[] = {0.5, 0.5, 0.5, 0.1};
float shiny2 = 10.0;


//Material properties for octohedrons
float matAmb3[] = {0.329412f, 0.223529f, 0.027451f,1.0f};
float matDiff3[] = {0.780392f, 0.568627f, 0.113725f, 1.0f};
float matSpec3[] = {0.992157f, 0.941176f, 0.807843f, 1.0f};
float shiny3 = 10.0;


//Material properties for spheres
float matAmb4[] = {0.0215f, 0.1745f, 0.0215f, 0.55f};
float matDiff4[] = {0.07568f, 0.61424f, 0.07568f, 0.55f};
float matSpec4[] = {0.633f, 0.727811f, 0.633f, 0.55f};
float shiny4 = 5;



// camera
const float CAM_MOVE_SPEED = 0.009;     // increase if you aren't getting 60 FPS
const float CAM_PAN_Y_SPEED = 50.0;
const float CAM_ROTATE_SPEED = 5;
const float FAR_PLANE_DIST = 1000.0;

// friction
const float FRICTION_LINEAR = 0.0009;
const float FRICTION_QUADRATIC = 0.006;
float ratio = 0;                        // ratio between speed components to preserve ratio while applying friction
float reduction = 0;                    // the "reduction" in the magnitude of the speed vector
float magScale = 0;                     // the scaling factor of the magnitude
bool frictionSwitch = true;             // toggle with 'r'

// gravity
const float GRAVITY_COEFF = 0.006;
bool gravitySwitch = true;             // toggle with 't'

// collision
const float TEST_CUBE_LENGTH = 5;
float testOffset[3] = {25, TEST_CUBE_LENGTH/2, 25};

// buildings
const int BUILDING_NUM = 30;
const float BUILDING_HEIGHT_MULT = 2;

// boosters
const int TOTAL_NUM_VERT_BOOST = 50;
const float VERT_SPEED_BOOST = 0.2;
const float VERT_BOOST_LENGTH = 1;
vector<vector<float>> boosterVerticalVerts = {
    {-VERT_BOOST_LENGTH/2, -VERT_BOOST_LENGTH/2, -VERT_BOOST_LENGTH/2},
    {-VERT_BOOST_LENGTH/2, -VERT_BOOST_LENGTH/2, VERT_BOOST_LENGTH/2},
    {-VERT_BOOST_LENGTH/2, VERT_BOOST_LENGTH/2, -VERT_BOOST_LENGTH/2},
    {VERT_BOOST_LENGTH/2, -VERT_BOOST_LENGTH/2, -VERT_BOOST_LENGTH/2},
    {-VERT_BOOST_LENGTH/2, VERT_BOOST_LENGTH/2, VERT_BOOST_LENGTH/2},
    {VERT_BOOST_LENGTH/2, -VERT_BOOST_LENGTH/2, VERT_BOOST_LENGTH/2},
    {VERT_BOOST_LENGTH/2, VERT_BOOST_LENGTH/2, -VERT_BOOST_LENGTH/2},
    {VERT_BOOST_LENGTH/2, VERT_BOOST_LENGTH/2, VERT_BOOST_LENGTH/2},
};
const int TOTAL_NUM_XZ_BOOST = 50;
const float XZ_SPEED_MULT = 1.15;
const float XZ_SPEED_BOOST = 0.2;
const float XZ_BOOST_RADIUS = 0.5;

// score
int score = 0;
const int VERT_BOOST_SCORE = 50;
const int XZ_BOOST_SCORE = 10;

// light source 1
float pos0[4] = {MAP_SIZE * CHECKER_SIZE, MAP_SIZE * CHECKER_SIZE/2, MAP_SIZE * CHECKER_SIZE, 0};
float amb0[4] = {1, 0.7, 0.7, 1};
float dif0[4] = {1, 0.7, 0.7, 1};
float spec0[4] = {1, 0.7, 0.7, 1};

// light source 2
float pos1[4] = {-MAP_SIZE * CHECKER_SIZE, MAP_SIZE * CHECKER_SIZE/2, -MAP_SIZE * CHECKER_SIZE, 0};
float amb1[4] = {0.7, 0.7, 1, 1};
float dif1[4] = {0.7, 0.7, 1, 1};
float spec1[4] = {0.7, 0.7, 1, 1};

// fog
float fogDensity = 0.02;
float fogColor[4] = {0, 1, 0, 1.0};
bool fogSwitch = false;

// ray picking stuff (selecting collidables, NPCs, etc)
vector<collidable> collidableList;
int selectedID;

// // bullet test
// float bulletDist = 0;
// bool bulletFired = false;
// const float BULLET_SIZE = 3;
// const float BULLET_SPEED = 10;

// ------- creates all collidables including the "car" HERE so that the code compiles -------
car test;
collidable test1;

// sky box
car skybox = *(new car(300));   // the * dereferences

// CAR LIGHT SOURCE
float pos2[4] = {test.pos[0], test.pos[1], test.pos[2]};
float amb2[4] = {0, 1, 0, 1};
float dif2[4] = {0, 1, 0, 1};
float spec2[4] = {0, 1, 0, 1};

// ------- INDEPENDENT FUNCTIONS -------

void startMessage(){
    printf("\n\n\n\nWelcome to BUMPER PARK 420!!!\n\n");
    printf("The objective of this game is to control the HOVER ROCKET across the terrain\n");
    printf("and collect as many POINTS as possible!! ISN'T THAT RAD?!\n");
    printf("Hit BOOSTER PADS to increase your score until you fall off the map!\n\n");
    printf(" === CONTROLS ===\n\n");
    printf("You can control the HOVER ROCKET with WASD, friction slows you down, so put the pedal to the CUBULAR metal.\n");
    printf("You can rotate left and right with the left and right arrow keys. This will help you not fall off the map!\n");
    printf("You can adjust the view up or down using the up and down arrow keys for some rad views from different heights!\n");
    printf("You can press X to return to origin in case you fall off the map, but you'll have to sacrifice your score!\n");
    printf("Of course, you can keep your score. But then you won't be able to continue playing our totally rad game.\n\n");
    printf("Clicking an object will tell you a message related to that object, to deselect click away from the object.\n");
    printf("y toggles on and off the fog, which is an added visual feature; this feature obscures the skybox however.\n");
    printf("t will toggle on and off gravity (takes away the difficulty but makes navigation somewhat easier)\n");
    printf("r will toggle on and off friction (again takes away some difficulty, but helps with navigation.\n\n");
    printf("Enjoy BUMPER PARK 420!!!\n\n\n");
}

float degToRad(float x) {
    return x * M_PI/180;
}

float vectorMagnitude(vector<float> v) {
    return sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
}

float vectorMagnitudeArray(float v[]) {
    return sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
}

float dotProduct(vector<float> v1, vector<float> v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

vector<float> crossProduct(vector<float> v1, vector<float> v2) {
    vector<float> v3;
    v3.push_back(v1[1]*v2[2] - v1[2]*v2[1]);
    v3.push_back(v1[2]*v2[0] - v1[0]*v2[2]);
    v3.push_back(v1[0]*v2[1] - v1[1]*v2[0]);
    return v3;
}

void rayInit(float endx, float endy, float endz){
    picker.start = {test.carEye[0], test.carEye[1], test.carEye[2]};
    picker.end = {endx, endy, endz};
    vector<float> rayVect;
    rayVect = {endx - picker.start[0], endy - picker.start[1], endz - picker.start[2]};
    picker.length = vectorMagnitude(rayVect);
    picker.dir = {rayVect[0] / picker.length, rayVect[1] / picker.length, rayVect[2] / picker.length};
}

void rayCast(int x, int y){
    double model[16], proj[16];
    int view[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, view);
    float winX = x;
    float winY = view[3] - y;
    
    vector<double> rayS = {0.0, 0.0, 0.0}; // start of the ray (S for Start)
    gluUnProject(winX, winY, 0.0, model, proj,
                 view, &rayS[0], &rayS[1], &rayS[2]); // convert to global coords
    
    vector<double> rayF = {0.0, 0.0, 0.0}; // end of the ray (F for Final)
    
    gluUnProject(winX, winY, 1.0, model, proj,
                 view, &rayF[0], &rayF[1], &rayF[2]); // convert to global coords
    
    picker.dir.push_back(rayF[0] - test.carEye[0]);
    picker.dir.push_back(rayF[1] - test.carEye[1]);
    picker.dir.push_back(rayF[2] - test.carEye[2]);
    
    rayInit(rayF[0], rayF[1], rayF[2]);
}

void drawRay(ray r) {               // draws the input ray
    if (r.start.size() == 3 && r.end.size() == 3){
        glBegin(GL_LINES);
        glVertex3f(r.start[0], r.start[1], r.start[2]);
        glVertex3f(r.end[0], r.end[1], r.end[2]);
        glEnd();
    }
}

void rayVsMap(ray r, vector<float> planeNormal, vector<float> planePos){
    float denom = dotProduct(planeNormal, r.dir);
    if (denom != 0){
        vector<float> offset = {r.start[0], r.start[1], r.start[2]};
        float t = -dotProduct(offset, planeNormal) / denom;
        
        vector<float> point = {
            t * r.dir[0] + r.start[0],
            t * r.dir[1] + r.start[1],
            t * r.dir[2] + r.start[2]
        };
        printf("%f, %f, %f\n", point[0], point[1], point[2]);
        if  ( !(planePos[0] - MAP_SIZE * CHECKER_SIZE/2 > point[0] or planePos[0] + MAP_SIZE * CHECKER_SIZE/2 < point[0]) and
             !(planePos[2] - MAP_SIZE * CHECKER_SIZE/2 > point[2] or planePos[2] + MAP_SIZE * CHECKER_SIZE/2 < point[2])) {
            printf("its a hit!");
        } else {
            printf("miss");
        }
    }
}

bool rayVsObj(ray r, collidable test){
    float t1 = 0;
    float t2 = 0;
    float temp;
    float tNear = -INFINITY;
    float tFar = INFINITY;
    
    for (int i = 0; i < 3; i++) {
        if (r.dir[i] == 0) {
            if (r.start[i] < test.boundBox[2 * i] || r.start[i] > test.boundBox[2 * i + 1]) {
                return false;
            }
        }
        else {
            t1 = (test.boundBox[2 * i] - r.start[i])/r.dir[i];
            t2 = (test.boundBox[2 * i + 1] - r.start[i])/r.dir[i];
            if (t1 > t2) {
                temp = t1;
                t1 = t2;
                t2 = temp;
            }
            if (t1 > tNear) {
                tNear = t1;
            }
            if (t2 < tFar) {
                tFar = t2;
            }
            if (tNear > tFar) {
                return false;
            }
            if (tFar < 0) {
                return false;
            }
        }
    }
    return true;
    
}

// void buildingGen(){
//     // ---- RANDOM HEIGHT -----
//     // height is set to a range between BUMP_HEIGHT and 2*BUMP_HEIGHT
//     float rHeight = BUMP_HEIGHT * BUILDING_HEIGHT_MULT * (rand()/RAND_MAX + 1);

//     // ---- RANDOM DEPTH/WIDTH -----
//     // depth and width are set to a range between 0.1*MAP_SIZE*CHECKER_SIZE and 0.4*MAP_SIZE*CHECKER_SIZE.
//     float scale =  0.1 * MAP_SIZE * CHECKER_SIZE;
//     float rDepth = scale * ((rand() /RAND_MAX) + 1);
//     float rWidth = scale * ((rand() /RAND_MAX) + 1);


//     float xRange = (MAP_SIZE * CHECKER_SIZE) - rWidth;
//     float zRange = (MAP_SIZE * CHECKER_SIZE) - rDepth;

//     float rXPos = (rand()) / (RAND_MAX / xRange)  + 0.5 * rWidth - 0.5 * MAP_SIZE * CHECKER_SIZE;
//     float rZPos = (rand()) / (RAND_MAX / zRange)  + 0.5 * rDepth - 0.5 * MAP_SIZE * CHECKER_SIZE;
//     float yPos = - BUMP_HEIGHT + 0.5 * rHeight;
//     //printf("%f, %f, %d\n",yPos, rHeight, ((rand() /RAND_MAX) + 1));
//     vector<vector<float>> buildingVerts = {
//         {-rWidth * 0.5, -rHeight * 0.5, -rDepth * 0.5}, //0
//         {-rWidth * 0.5, -rHeight * 0.5, rDepth * 0.5},  //1
//         {-rWidth * 0.5, rHeight * 0.5, -rDepth * 0.5},  //2
//         {rWidth * 0.5, -rHeight * 0.5, -rDepth * 0.5}, ///3
//         {-rWidth * 0.5, rHeight * 0.5, rDepth * 0.5},  ///4
//         {rWidth * 0.5, -rHeight * 0.5, rDepth * 0.5}, ////5
//         {rWidth * 0.5, rHeight * 0.5, -rDepth * 0.5}, ////6
//         {rWidth * 0.5, rHeight * 0.5, rDepth * 0.5},  ////7
//     };

//     vector<vector<int>> buildingIndices = {
//         {1,4,2,0},
//         {5,7,4,1},
//         {3,6,7,5},
//         {0,2,6,3},
//         {2,4,7,6},
//         {0,3,5,1}
//     };
//     //printf("aaaaaaa");
//     vector<float> buildPos = {rXPos, yPos, rZPos};
//     collidable building = collidable(buildPos, {0,0,0}, false);
//     building.setVerts(buildingVerts);
//     building.indices = buildingIndices;
//     building.setBoundBox(buildingVerts);
//     building.calcFaceNorms();
//     building.setName("What're you looking at.");
//     collidableList.push_back(building);
// }

// projects a vector onto a plane and returns the vector result.
// the y component, if +ve, should update the cubes y velocity.
// projecting gravity onto the plane, take the zx component to give the slide of the cube
vector<float> getPlanarComp(vector<float> v, vector<float> n) {
    float dotResult = dotProduct(v, n);
    float length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    vector<float> result;
    for (int i = 0; i < 3; i++) {
        result.push_back(v[i] - n[i] * dotResult / length);
    }
    return result;
}

void bumpIt() {
    
    int tempX = 0;
    int tempZ = 0;
    int lowX = 0;
    int highX = 0;
    int lowZ = 0;
    int highZ = 0;
    float dist = 0;         // distance of vertex from circle center
    int upOrDown = 0;
    vector<float> temp;     // temp vector for initializing height field
    
    for (int i = 0; i < MAP_SIZE; i++) {        // initialize height field
        for (int j = 0; j < MAP_SIZE; j++) {
            temp.push_back(0);
        }
        heights.push_back(temp);
    }
    
    for (int bumps = 0; bumps < NUM_BUMPS; bumps++) {
        
        // circle center coordinates
        tempX = (MAP_SIZE - 1) - rand() % MAP_SIZE;
        tempZ = (MAP_SIZE - 1) - rand() % MAP_SIZE;
        
        // flag for bumping up or down
        upOrDown = rand() % 2;
        upOrDown = (upOrDown - 0.5) * 2;
        
        // checks which vertices inside box are within the circle and changes y value based on proximity to circle center
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                dist = sqrt(pow(i - tempX, 2) + pow(j - tempZ, 2));
                if (dist < BUMP_RADIUS) {
                    heights[i][j] += BUMP_HEIGHT * upOrDown * cos(M_PI * dist/(2.0 * BUMP_RADIUS));
                }
            }
        }
        
    }
    
}

void initMap() {
    // temp vectors
    vector<float> v1;
    vector<float> v2;
    vector<float> v3;
    for (int x = 0; x < MAP_SIZE; x++) {
        for (int z = 0; z < MAP_SIZE; z++) {
            map[x][z][0] = (x - MAP_SIZE/2) * CHECKER_SIZE;
            map[x][z][1] = heights[x][z];
            map[x][z][2] = (z - MAP_SIZE/2) * CHECKER_SIZE;
        }
    }
    
    // FACE NORMALS
    for (int x = 0; x < MAP_SIZE - 1; x++) {
        for (int z = 0; z < MAP_SIZE - 1; z++) {
            v1.push_back(CHECKER_SIZE);                                 // x
            v1.push_back(heights[x + 1][z + 1] - heights[x][z + 1]);    // y
            v1.push_back(0);                                            // z
            v2.push_back(0);
            v2.push_back(heights[x + 1][z] - heights[x + 1][z + 1]);
            v2.push_back(-CHECKER_SIZE);
            v3 = crossProduct(v1, v2);
            for (int i = 0; i < 3; i++) {
                faceNorms[x][z][i] = v3[i];        // assign face normals
            }
            while (v1.size() != 0) {
                v1.pop_back();
            }
            while (v2.size() != 0) {
                v2.pop_back();
            }
        }
    }
    while (v3.size() != 0) {v3.pop_back();}
    
    // VERTEX NORMALS (NEED TO CONSIDER INTERIOR, OUTSIDE EDGES AND FOUR CORNERS!)
    // interior
    vector<float> temp;
    for (int x = 1; x < MAP_SIZE - 1; x++) {
        for (int z = 1; z < MAP_SIZE - 1; z++) {
            for (int i = 0; i < 3; i++) {
                temp.push_back(faceNorms[x - 1][z - 1][i] + faceNorms[x][z - 1][i] + faceNorms[x][z][i] + faceNorms[x - 1][z][i]);
            }
            for (int i = 0; i < 3; i++) {   // needs to fill up temp first before calculating vertex norms
                vertexNorms[x][z][i] = temp[i]/vectorMagnitude(temp);
            }
            while (temp.size() != 0) {
                temp.pop_back();
            }
        }
    }
    // outside edges excluding corners
    for (int x = 0; x < MAP_SIZE; x++) {
        for (int z = 0; z < MAP_SIZE; z++) {
            if (x == 0 || x == MAP_SIZE-1 || z == 0 || z == MAP_SIZE-1) {   // if outside edge
                if ((x + z) % (MAP_SIZE-1) != 0 && (x == 0 || x == MAP_SIZE-1)) {    // bottom or top edges not corner
                    for (int i = 0; i < 3; i++) {
                        temp.push_back(faceNorms[x][z][i] + faceNorms[x][z - 1][i]);
                    }
                    for (int i = 0; i < 3; i++) {
                        vertexNorms[x][z][i] = temp[i]/vectorMagnitude(temp);
                    }
                    while (temp.size() != 0) {
                        temp.pop_back();
                    }
                }
                else if ((x + z) % (MAP_SIZE-1) != 0 && (z == 0 || z == MAP_SIZE-1)) {   // left or right edges not corner
                    for (int i = 0; i < 3; i++) {
                        temp.push_back(faceNorms[x][z][i] + faceNorms[x-1][z][i]);
                    }
                    for (int i = 0; i < 3; i++) {
                        vertexNorms[x][z][i] = temp[i]/vectorMagnitude(temp);
                    }
                    while (temp.size() != 0) {
                        temp.pop_back();
                    }
                }
            }
        }
    }
    // corners
    for (int i = 0; i < 3; i++) {
        vertexNorms[0][0][i] = faceNorms[0][0][i]/vectorMagnitudeArray(faceNorms[0][0]);
        vertexNorms[0][MAP_SIZE-1][i] = faceNorms[0][MAP_SIZE-2][i]/vectorMagnitudeArray(faceNorms[0][MAP_SIZE-2]);
        vertexNorms[MAP_SIZE-1][0][i] = faceNorms[MAP_SIZE-2][0][i]/vectorMagnitudeArray(faceNorms[MAP_SIZE-2][0]);
        vertexNorms[MAP_SIZE-1][MAP_SIZE-1][i]
        = faceNorms[MAP_SIZE-2][MAP_SIZE-2][i]/vectorMagnitudeArray(faceNorms[MAP_SIZE-2][MAP_SIZE-2]);
    }
    
}

void drawMap() {
    for (int x = 0; x < MAP_SIZE-1; x++)
    {
        for (int z = 0; z < MAP_SIZE-1; z++)
        {
            float checker = (x + z) % 2;
            glBegin(GL_QUADS);
            glColor3f(checker, checker, checker);
            glNormal3fv(vertexNorms[x][z]);
            glVertex3fv(map[x][z]);
            glNormal3fv(vertexNorms[x+1][z]);
            glVertex3fv(map[x+1][z]);
            glNormal3fv(vertexNorms[x+1][z+1]);
            glVertex3fv(map[x+1][z+1]);
            glNormal3fv(vertexNorms[x][z+1]);
            glVertex3fv(map[x][z+1]);
            glEnd();
        }
    }
}

bool checkBoundary(float inX, float inZ) {
    if (inX >= -MAP_SIZE/2 * CHECKER_SIZE && inX <= (MAP_SIZE/2 - 1) * CHECKER_SIZE
        && inZ >= -MAP_SIZE/2 * CHECKER_SIZE && inZ <= (MAP_SIZE/2 - 1) * CHECKER_SIZE) {
        return true;
    }
    else {
        return false;
    }
}

float calcY(float inX, float inZ) {     /* uses x and z coordinates to find the vertice index
                                         which can be used to find the y value at that index to match
                                         the car with the terrain */
    
    insideMap = checkBoundary(inX, inZ);
    if (insideMap) {
        return heights[(int) round(inX/CHECKER_SIZE + MAP_SIZE/2)][(int) round(inZ/CHECKER_SIZE + MAP_SIZE/2)];
    }
    else {
        return 0;
    }
}

void adjustSpeed(vector<float> inV) {
    
    vector<float> vNorm;
    vector<float> projSpeed;                // projection of speed onto vertex normal plane
    vector<float> projGravity;              // projection of gravity vector (0, -1, 0) ??? onto vertex normal plane
    vector<float> gravityVector;            // dummy, play with it later if needed
    gravityVector.push_back(0);
    gravityVector.push_back(-GRAVITY_COEFF);
    gravityVector.push_back(0);
    
    for (int i = 0; i < 3; i++) {
        vNorm.push_back(vertexNorms[(int) round(inV[0]/CHECKER_SIZE + MAP_SIZE/2)][(int) round(inV[2]/CHECKER_SIZE + MAP_SIZE/2)][i]);
        printf("%f,%f,%f\n", vNorm[0], vNorm[1], vNorm[2]);
    }
    for (int i = 0; i < 3; i++) {
        projSpeed.push_back(getPlanarComp(vNorm, test.speed)[i]);
        projGravity.push_back(getPlanarComp(vNorm, gravityVector)[i]);
    }
    test.speed[0] = test.speed[0] - projGravity[0];
    test.speed[1] += projSpeed[1];
    test.speed[2] = test.speed[2] - projGravity[2];
    
}

void drawAxis() {               // just for visual debugging
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(-FAR_PLANE_DIST, 0, 0);
    glVertex3f(FAR_PLANE_DIST, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, -FAR_PLANE_DIST, 0);
    glVertex3f(0, FAR_PLANE_DIST, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, -FAR_PLANE_DIST);
    glVertex3f(0, 0, FAR_PLANE_DIST);
    glEnd();
}

void drawHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, MAX_X, 0, MAX_Y);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3fv(HUD_COLOR);
    glRasterPos2f(HUD_SPEED_X_POS, HUD_SPEED_Y_POS);
    string speedText = "SPEED: ";
    for (int i = 0; i < speedText.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, speedText[i]);
    }
    glRasterPos2f(HUD_SPEED_X_POS + HUD_SPACING, HUD_SPEED_Y_POS);
    string speedValueText = to_string(test.magnitude);
    for (int i = 0; i < speedValueText.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, speedValueText[i]);
    }
    glRasterPos2f(HUD_ANGLE_X_POS, HUD_ANGLE_Y_POS);
    string angleText = "ANGLE: ";
    for (int i = 0; i < angleText.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, angleText[i]);
    }
    glRasterPos2f(HUD_ANGLE_X_POS + HUD_SPACING, HUD_ANGLE_Y_POS);
    string angleValueText = to_string(test.angle[1]);
    for (int i = 0; i < angleValueText.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, angleValueText[i]);
    }
    if (collidableList.size() != 0 && selectedID != 0) {
        glRasterPos2f(HUD_OBJNAME_X_POS, HUD_OBJNAME_Y_POS);
        string angleText = collidableList[selectedID].name;
        for (int i = 0; i < angleText.length(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, angleText[i]);
        }
    }
    glRasterPos2f(HUD_SCORE_X_POS, HUD_SCORE_Y_POS);
    string scoreText = "SCORE: ";
    for (int i = 0; i < scoreText.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreText[i]);
    }
    glRasterPos2f(HUD_SCORE_X_POS + HUD_SPACING, HUD_SCORE_Y_POS);
    string scoreValueText = to_string(score);
    for (int i = 0; i < scoreValueText.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreValueText[i]);
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
}

// ------- COLLISION EFFECTS -------
void collideEffects(int type) {
    if (type == 2) {                // powerups
        
    }
    else if (type == 3) {                // vertical boost
        test.speed[1] += VERT_SPEED_BOOST;
        score += VERT_BOOST_SCORE;
    }
    else if (type == 4) {               // XZ boost
        test.speed[0] = test.speed[0] * XZ_SPEED_MULT;
        test.speed[2] = test.speed[2] * XZ_SPEED_MULT;
        score += XZ_BOOST_SCORE;
    }
    else {                          // REGULAR COLLISION
        test.speed[0] = -1 * test.speed[0];
    }
}

// ------- KEYBOARD -------
void keyboard(unsigned char key, int x, int y) {
    int mod = glutGetModifiers();
    switch (key)
    {
        case 'q':        // press q to quit
            exit(0);
            break;
        case 'w':        // move x forward
            test.speed[0] += CAM_MOVE_SPEED * cos(degToRad(test.angle[1]));
            test.speed[2] += CAM_MOVE_SPEED * sin(degToRad(test.angle[1]));
            // printf("Speed X: %f\n", test.speed[0]);
            // printf("Speed Y: %f\n", test.speed[1]);
            // printf("Speed Z: %f\n", test.speed[2]);
            break;
        case 's':        // move x backward
            test.speed[0] -= CAM_MOVE_SPEED * cos(degToRad(test.angle[1]));
            test.speed[2] -= CAM_MOVE_SPEED * sin(degToRad(test.angle[1]));
            // printf("Speed X: %f\n", test.speed[0]);
            // printf("Speed Y: %f\n", test.speed[1]);
            // printf("Speed Z: %f\n", test.speed[2]);
            break;
        case 'd':        // move z forward
            test.speed[0] -= CAM_MOVE_SPEED * sin(degToRad(test.angle[1]));
            test.speed[2] += CAM_MOVE_SPEED * cos(degToRad(test.angle[1]));
            // printf("Speed X: %f\n", test.speed[0]);
            // printf("Speed Y: %f\n", test.speed[1]);
            // printf("Speed Z: %f\n", test.speed[2]);
            break;
        case 'a':        // move z backward
            test.speed[0] += CAM_MOVE_SPEED * sin(degToRad(test.angle[1]));
            test.speed[2] -= CAM_MOVE_SPEED * cos(degToRad(test.angle[1]));
            // printf("Speed X: %f\n", test.speed[0]);
            // printf("Speed Y: %f\n", test.speed[1]);
            // printf("Speed Z: %f\n", test.speed[2]);
            break;
        case ' ':        // freezes cube
            for (int i = 0; i < 3; i++) {
                test.speed[i] = 0;
            }
            printf("EMERGENCY MAGIC BRAKE! Press 'x' to reset.\n");
            break;
        case 'x':        // resets everything
            for (int i = 0; i < 3; i++) {
                test.carEye[i] = test.EYE_START[i];
                test.carFocus[i] = test.FOCUS_START[i];
                test.pos[i] = test.POS_START[i];
                test.speed[i] = 0;
            }
            test.angle[1] = 0;
            score = 0;
            insideMap = true;
            printf("RESET!\n");
            break;
        case 'f':           // PRESS TO CHECK CURRENT POSITION!
            printf("Pos X: %f\n", test.pos[0]);
            printf("Pos Y: %f\n", test.pos[1]);
            printf("Pos Z: %f\n", test.pos[2]);
            break;
        case 'g':           // PRESS TO CHECK CURRENT SPEED!
            printf("SPEED X: %f\n", test.speed[0]);
            printf("SPEED Y: %f\n", test.speed[1]);
            printf("SPEED Z: %f\n", test.speed[2]);
            break;
        case 'r':
            if (frictionSwitch) {
                frictionSwitch = false;
                printf("TURNING OFF FRICTION\n");
            }
            else {
                frictionSwitch = true;
                printf("TURNING ON FRICTION\n");
            }
            break;
        case 't':
            if (gravitySwitch) {
                gravitySwitch = false;
                printf("TURNING OFF GRAVITY\n");
            }
            else {
                gravitySwitch = true;
                printf("TURNING ON GRAVITY\n");
            }
            break;
        case 'y':
            if (fogSwitch) {
                glDisable (GL_FOG);
                fogSwitch = false;
                printf("TURNING OFF FOG\n");
            }
            else {
                glEnable (GL_FOG);
                fogSwitch = true;
                printf("TURNING ON FOG\n");
            }
            break;
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(0, 0, 0, 0);
    glColor3f(1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, FAR_PLANE_DIST);
    
    glFogi (GL_FOG_MODE, GL_EXP2);
    glFogfv (GL_FOG_COLOR, fogColor);
    glFogf (GL_FOG_DENSITY, fogDensity);
    glHint (GL_FOG_HINT, GL_NICEST);
    
    // CULLING
    glFrontFace(GL_CCW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    
    glEnable(GL_TEXTURE_2D);
    glGenTextures(2, textureID);
    
    metal_tex = LoadPPM("metalFloor.ppm", &width, &height, &maximum);
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, metal_tex);
    //set texture parameters
    space_tex = LoadPPM("space2.ppm", &width, &height, &maximum);
    glBindTexture(GL_TEXTURE_2D, textureID[1]);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, space_tex);
    
    ice_tex = LoadPPM("ice.ppm", &width, &height, &maximum);
    glBindTexture(GL_TEXTURE_2D, textureID[2]);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, ice_tex);
    
    // create the checkered map and calculate face normals for the car
    bumpIt();
    initMap();
    test.calcFaceNorms();
    skybox.calcFaceNorms();
    startMessage();
    
    // pushes every collidable onto the collidable list vector (EVERY collidable needs to be on this vector!)
    collidableList.push_back(test);
    collidableList.push_back(test1);
    vector<float> tempPos;
    vector<float> tempAngle;
    for (int i = 0; i < TOTAL_NUM_VERT_BOOST; i++) {        // set up vertical boosts
        tempPos.push_back(((rand() % MAP_SIZE) - MAP_SIZE/2) * CHECKER_SIZE);
        tempPos.push_back(0);
        tempPos.push_back(((rand() % MAP_SIZE) - MAP_SIZE/2) * CHECKER_SIZE);
        for (int j = 0; j < 3; j++) {
            tempAngle.push_back(0);
        }
        collidable temp(tempPos, tempAngle, 3);
        collidableList.push_back(temp);
        collidableList[collidableList.size() - 1].setVerts(boosterVerticalVerts);
        collidableList[collidableList.size() - 1].setBoundBox(boosterVerticalVerts);
        collidableList[collidableList.size() - 1].setName("come at me bro");
        tempPos.clear();
        tempAngle.clear();
    }
    for (int i = 0; i < TOTAL_NUM_XZ_BOOST; i++) {          // set up XZ boosts
        tempPos.push_back(((rand() % MAP_SIZE) - MAP_SIZE/2) * CHECKER_SIZE);
        tempPos.push_back(0);
        tempPos.push_back(((rand() % MAP_SIZE) - MAP_SIZE/2) * CHECKER_SIZE);
        for (int j = 0; j < 3; j++) {
            tempAngle.push_back(0);
        }
        collidable temp(tempPos, tempAngle, 4);
        collidableList.push_back(temp);
        for (int j = 0; j < 6; j++) {       // alternate between negative and positive and translate
            collidableList[collidableList.size() - 1].boundBox.push_back(XZ_BOOST_RADIUS * 2 * ((j % 2) - 0.5) + tempPos[j / 2]);
        }
        collidableList[collidableList.size() - 1].setName("come here for a RUSH");
        tempPos.clear();
        tempAngle.clear();
    }
    // for (int i = 0; i < BUILDING_NUM; i++){
    //     buildingGen();
    // }
    
    
    // ------- SETTING UP TEST 1 -------
    vector<vector<float>> tempVerts = {
        {-TEST_CUBE_LENGTH/2 + testOffset[0], -TEST_CUBE_LENGTH/2 + testOffset[1], -TEST_CUBE_LENGTH/2 + testOffset[2]},
        {-TEST_CUBE_LENGTH/2 + testOffset[0], -TEST_CUBE_LENGTH/2 + testOffset[1], TEST_CUBE_LENGTH/2 + testOffset[2]},
        {-TEST_CUBE_LENGTH/2 + testOffset[0], TEST_CUBE_LENGTH/2 + testOffset[1], -TEST_CUBE_LENGTH/2 + testOffset[2]},
        {TEST_CUBE_LENGTH/2 + testOffset[0], -TEST_CUBE_LENGTH/2 + testOffset[1], -TEST_CUBE_LENGTH/2 + testOffset[2]},
        {-TEST_CUBE_LENGTH/2 + testOffset[0], TEST_CUBE_LENGTH/2 + testOffset[1], TEST_CUBE_LENGTH/2 + testOffset[2]},
        {TEST_CUBE_LENGTH/2 + testOffset[0], -TEST_CUBE_LENGTH/2 + testOffset[1], TEST_CUBE_LENGTH/2 + testOffset[2]},
        {TEST_CUBE_LENGTH/2 + testOffset[0], TEST_CUBE_LENGTH/2 + testOffset[1], -TEST_CUBE_LENGTH/2 + testOffset[2]},
        {TEST_CUBE_LENGTH/2 + testOffset[0], TEST_CUBE_LENGTH/2 + testOffset[1], TEST_CUBE_LENGTH/2 + testOffset[2]},
    };
    collidableList[1].setVerts(tempVerts);
    collidableList[1].setBoundBox(tempVerts);
    collidableList[1].setName("BOX OF BROKEN DREAMS");
    
}

// ------- DISPLAY -------
void display() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_POSITION, pos0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT1, GL_POSITION, pos1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, dif1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
    glEnable(GL_LIGHT1);
    // glLightfv(GL_LIGHT2, GL_POSITION, pos2);
    // glLightfv(GL_LIGHT2, GL_DIFFUSE, dif2);
    // glLightfv(GL_LIGHT2, GL_AMBIENT, amb2);
    // glLightfv(GL_LIGHT2, GL_SPECULAR, spec2);
    // glEnable(GL_LIGHT2);
    // glEnable(GL_COLOR_MATERIAL);
    
    // movable camera
    gluLookAt(test.carEye[0], test.carEye[1], test.carEye[2], test.carFocus[0], test.carFocus[1], test.carFocus[2], 0, 1, 0);
    drawAxis();
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec0);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny0);
    glBindTexture(GL_TEXTURE_2D, textureID[2]);
    drawMap();
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec1);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny1);
    glBindTexture(GL_TEXTURE_2D, textureID[1]);
    skybox.drawCar();
    
    // set up bounding box for "car"
    if (test.verts.size() != 0) {
        test.setBoundBox(test.verts);
    }
    
    // draw the car
    glPushMatrix();
    glTranslatef(test.pos[0], test.pos[1], test.pos[2]);
    glRotatef(-test.angle[0], 1, 0, 0);
    glRotatef(-test.angle[1], 0, 1, 0);
    glRotatef(-test.angle[2], 0, 0, 1);
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    test.drawCar();
    glPopMatrix();
    
    // update position
    for (int i = 0; i < 3; i ++) {
        test.pos[i] += test.speed[i];
    }
    
    // adjust the y position if inside map
    if (insideMap) {
        onMap = false;
        if (test.pos[1] < calcY(test.pos[0], test.pos[2]) + test.OBJ_START_HEIGHT + ON_MAP_LIMIT) {
            test.pos[1] = calcY(test.pos[0], test.pos[2]) + test.OBJ_START_HEIGHT;
            test.speed[1] = 0;
            onMap = true;
        }
    }
    
    // for rotating carEye / carFocus
    test.carEye[0] = test.pos[0] + test.EYE_START[0]*cos(degToRad(test.angle[1]));
    test.carEye[1] = test.pos[1] + test.cubeCamOffset[1];
    test.carEye[2] = test.pos[2] + test.EYE_START[2]*sin(degToRad(test.angle[1]));
    
    test.carFocus[0] = test.pos[0] + test.FOCUS_START[0]*cos(degToRad(test.angle[1]));
    test.carFocus[1] = test.pos[1];
    test.carFocus[2] = test.pos[2] + test.FOCUS_START[2]*sin(degToRad(test.angle[1]));
    
    // calculates magnitude (need to display it anyways)
    test.magnitude = sqrt(pow(test.speed[0], 2) + pow(test.speed[1], 2) + pow(test.speed[2], 2));
    
    // ------- FRICTION -------
    if (frictionSwitch) {
        
        reduction = FRICTION_LINEAR + test.magnitude * FRICTION_QUADRATIC;
        magScale = (test.magnitude - reduction)/test.magnitude;
        if (magScale < 0) {
            magScale = 0;
            test.magnitude = 0;
        }
        else {
            test.magnitude -= reduction;
        }
        
        for (int i = 0; i < 3; i++) {
            test.speed[i] = magScale * test.speed[i];
        }
        
    }
    
    // ------- GRAVITY -------
    if (gravitySwitch) {
        test.speed[1] -= GRAVITY_COEFF;
    }
    
    collidableList[1].drawBoundBox();
    
    // ------- draw the rest of the collidables  -------
    for (int i = 2; i < collidableList.size(); i++) {
        glColor3f(1, 0, 0);
        if (collidableList[i].type == 3) {
            glPushMatrix();
            glTranslatef(collidableList[i].pos[0], collidableList[i].pos[1], collidableList[i].pos[2]);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb3);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff3);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec3);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny3);
            glutSolidOctahedron();
            glPopMatrix();
        }
        else if (collidableList[i].type == 4) {
            glPushMatrix();
            glTranslatef(collidableList[i].pos[0], collidableList[i].pos[1], collidableList[i].pos[2]);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb4);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff4);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec4);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny4);
            glutSolidSphere(XZ_BOOST_RADIUS, 16, 16);
            glPopMatrix();
        }
        if (i == selectedID) {
            collidableList[i].drawBoundBox();
        }
    }
    
    for (int i = 1; i < collidableList.size(); i++) {
        if (test.collide(collidableList[i])) {
            collideEffects(collidableList[i].type);
        }
    }
    
    // ------- HUD ------- (BROKEN CURRENTLY BUT WE CAN USE THIS TO DRAW TEXT ON THE MAP, ONCE THE FOR LOOP TERMINATES!)
    drawHUD();
    
    glutSwapBuffers();
}

// ------- ARROW KEY CONTROLS (SPECIAL) -------

void special(int key, int x, int y) {    // camera rotations
    switch(key)
    {
        case GLUT_KEY_DOWN:
            test.cubeCamOffset[1] -= CAM_MOVE_SPEED * CAM_PAN_Y_SPEED;
            test.carFocus[1] -= CAM_MOVE_SPEED * CAM_PAN_Y_SPEED;
            break;
        case GLUT_KEY_UP:
            test.cubeCamOffset[1] += CAM_MOVE_SPEED * CAM_PAN_Y_SPEED;
            test.carFocus[1] += CAM_MOVE_SPEED * CAM_PAN_Y_SPEED;
            break;
        case GLUT_KEY_LEFT:
            test.angle[1] -= CAM_ROTATE_SPEED;
            if (test.angle[1] < 0) {
                test.angle[1] += 360;
            }
            break;
        case GLUT_KEY_RIGHT:
            test.angle[1] += CAM_ROTATE_SPEED;
            if (test.angle[1] >= 360) {
                test.angle[1] -= 360;
            }
            break;
    }
    glutPostRedisplay();
}

void mouse(int btn, int state, int x, int y) {
    if (btn == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            rayCast(x,y);
        }
        selectedID = 0;
        for (int i = 1; i < collidableList.size(); i++) {       // the car is the 0th index in the vector
            if (rayVsObj(picker, collidableList[i])) {
                selectedID = i;
                break;
            }
        }
    }
}

void reshape(int w, int h) {
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, ((w+0.0f)/h), 1, FAR_PLANE_DIST);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
}

void FPS(int val) {
    glutPostRedisplay();
    glutTimerFunc(17, FPS, 0);
}

void callBackInit() {
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, FPS, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    
    glutInitWindowSize(MAX_X, MAX_Y);
    glutInitWindowPosition(INITIAL_X_POS, INITIAL_Y_POS);
    
    glutCreateWindow("Main");
    
    srand((unsigned) time(NULL) * getpid());
    
    init();
    callBackInit();
    glEnable(GL_NORMALIZE);
    
    glutMainLoop();
    
    return(0);
}
