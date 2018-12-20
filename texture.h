//
//  texture.h
//  bumperParkthe2nd
//
//  Created by Danny Stewart on 2018-12-08.
//  Copyright © 2018 Danny Stewart. All rights reserved.
//

#ifndef texture_h
#define texture_h

#include <iostream>
#include <stdlib.h>
#include <stdio.h>



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

GLubyte* metal_tex;
GLubyte* space_tex;
GLubyte* ice_tex;

int width, height, maximum;

//char filename[50] = "./marble.ppm";

GLuint textureID[3];

GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
    GLubyte* img;
    FILE *fd;
    int n, m;
    int  k, nm;
    char c;
    int i;
    unsigned char b[100];
    float s;
    int red, green, blue;
    
    //reminder that you have a file handle, not a file name
    
    fd = fopen(file, "r");
    
    fscanf(fd," %[^\n] ",b);
    printf("b[0] : %u\n", b[0]);
    printf("b[1] : %u\n", b[1]);
    if(b[0]!='P'|| b[1] != '3' )
    {
        printf("%s is not a PPM file!\n",file);
        exit(0);
    }
    printf("%s is a PPM file\n", file);
    fscanf(fd, "%c",&c);
    while(c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
    }
    ungetc(c,fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);
    
    printf("%d rows  %d columns  max value= %d\n",n,m,k);
    
    nm = n*m;
    
    img = (GLubyte*)malloc(3*sizeof(GLuint)*nm);
    
    
    s=255.0/k;
    
    
    for(i=0;i<nm;i++)
    {
        fscanf(fd,"%d %d %d",&red, &green, &blue );
        img[3*nm-3*i-3]=red*s;
        img[3*nm-3*i-2]=green*s;
        img[3*nm-3*i-1]=blue*s;
    }
    
    *width = n;
    *height = m;
    *max = k;
    
    return img;
}

#endif /* texture_h */
