/*
 * 090mainScene.c
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * main program to test the transition from 2D to 3D
 */

#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 1000

#define renATTRDIM 8
#define renATTRX 0
#define renATTRY 1
#define renATTRZ 2
#define renATTRS 3
#define renATTRT 4
#define renATTRR 5
#define renATTRG 6
#define renATTRB 7

#define renVARYDIM 8
#define renVARYX 0
#define renVARYY 1
#define renVARYZ 2
#define renVARYS 3
#define renVARYT 4
#define renVARYR 5
#define renVARYG 6
#define renVARYB 7

#define renUNIFDIM 25
#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renUNIFTRANSX 3
#define renUNIFTRANSY 4
#define renUNIFTRANSZ 5
#define renUNIFALPHA 6
#define renUNIFPHI 7
#define renUNIFTHETA 8
#define renUNIFMAT00 9

#define texNUM 1
#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include "000pixel.h"
#include "100vector.c"
#include "100matrix.c"
#include "040texture.c"
#include "110depth.c"
#include "110renderer.c"
#include "110triangle.c"
#include "100mesh.c"
#include "090scene.c"


double unif[renUNIFDIM] = {0};
renRenderer ren;
int width = 512;
int height = 512;

texTexture *tex[texNUM];
sceneNode root;
sceneNode branch1;
sceneNode branch2;
sceneNode branch3;
sceneNode branch4;

/* If unifParent is NULL, then sets the uniform matrix to the
rotation-translation M described by the other uniforms. If unifParent is not
NULL, but instead contains a rotation-translation P, then sets the uniform
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    if (unifParent == NULL) {
        double rotation[3][3];
        double rho = 1;
        double axis[3] = {(rho * sin(unif[renUNIFPHI]) * cos(unif[renUNIFTHETA])),
                          (rho * sin(unif[renUNIFPHI]) * sin(unif[renUNIFTHETA])),
                          (rho * cos(unif[renUNIFPHI]))};
        mat33AngleAxisRotation(unif[renUNIFALPHA], axis, rotation);
        double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};
        // vecPrint(3, trans);
        mat44Isometry(rotation, trans, (double(*)[4])(&unif[renUNIFMAT00]));

        // printf("======== Transformation Matrix ========\n");
        // mat44Print((double(*)[4])(&unif[renUNIFMAT00]));

    } else {
      double rotation[3][3];
      double rho = 1;
      double axis[3] = {(rho * sin(unif[renUNIFPHI]) * cos(unif[renUNIFTHETA])),
                        (rho * sin(unif[renUNIFPHI]) * sin(unif[renUNIFTHETA])),
                        (rho * cos(unif[renUNIFPHI]))};
      mat33AngleAxisRotation(unif[renUNIFALPHA], axis, rotation);
      double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};
      // vecPrint(3, trans);
      double temp[4][4];
      mat44Isometry(rotation, trans, temp);
      mat444Multiply((double(*)[4])(&unifParent[renUNIFMAT00]),
                     temp,
                     (double(*)[4])(&unif[renUNIFMAT00]));
    }
}

/* Sets rgb, based on the other parameters, which are unaltered. attr is an
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[],
                double vary[], double rgbz[]) {
  texSample(tex[0], vary[renVARYS], vary[renVARYT]);
  rgbz[0] = tex[0]->sample[renTEXR];
  rgbz[1] = tex[0]->sample[renTEXG];
  rgbz[2] = tex[0]->sample[renTEXB];
  // printf("varyz %f\n", vary[renVARYZ]);
  rgbz[3] = vary[renVARYZ];
  // printf("colorPixel\n");
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[],
        double vary[]) {
    // printf("transformVertex\n");
    double temp[4];
    double expandedAttr[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 1};
    mat441Multiply((double(*)[4])(&unif[renUNIFMAT00]), expandedAttr, temp);
    vary[renATTRX] = temp[0];
    vary[renATTRY] = temp[1];
    vary[renATTRZ] = temp[2];
    vary[renVARYS] = attr[renATTRS];
    // printf("varyxy: %f, %f\n", vary[renATTRX],vary[renATTRY]);
    vary[renVARYT] = attr[renATTRT];
}


void handleTimeStep(double oldTime, double newTime) {
  if (floor(newTime) - floor(oldTime) >= 1.0) {
    printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
  }
  // printf("handleTimeStep\n");
  pixClearRGB(0, 0, 0);
  depthClearZs(ren.depth, -99999);
  unif[renUNIFTHETA] += .05;
  unif[renUNIFALPHA] += .05;
  unif[renUNIFPHI] += .05;
  sceneSetUniform(&root, &ren, unif);
  // printf("sceneSetUniform finished\n");
  sceneRender(&root, &ren, NULL);
  // sceneRender(&root, &ren, unif);
  // printf("sceneRender finshed\n");
}

int main() {
  if (pixInitialize(width, height, "Pixel Graphics") != 0) {
    return 1;
  } else {
    pixClearRGB(0, 0, 0);
    depthBuffer dBuffer;
    depthInitialize(&dBuffer, width, height);
    // int depthInitialize(depthBuffer *buf, int width, int height)

    // initialize ren
    ren.depth = &dBuffer;
    ren.attrDim = 8;
    ren.texNum = 1;
    ren.unifDim = renUNIFDIM;
    ren.varyDim = renVARYDIM;
    ren.transformVertex = transformVertex;
    ren.colorPixel = colorPixel;
    ren.updateUniform = updateUniform;

    // initialize textures`
    texTexture tex1;
    char *path1 = "cat1.jpg";
    texInitializeFile(&tex1, path1);
    texSetFiltering(&tex1, texQUADRATIC);
    texSetTopBottom(&tex1, texREPEAT);
    texSetLeftRight(&tex1, texREPEAT);
    tex[0] = &tex1;

    unif[renUNIFTRANSX] = 200;
    unif[renUNIFTRANSY] = 200;
    unif[renUNIFTRANSZ] = -100;
    unif[renUNIFALPHA] = 0;
    unif[renUNIFTHETA] = 0;
    unif[renUNIFPHI] = 0;

    // initialize some meshes
    meshMesh mesh1;
    meshInitializeSphere(&mesh1, 150, 20, 40);
    meshMesh mesh2;
    meshInitializeSphere(&mesh2, 20, 20, 40);

    sceneInitialize(&branch1, &ren, unif, tex, &mesh2, NULL, NULL);
    sceneSetOneUniform(&branch1, renUNIFTRANSX, 80);
    sceneSetOneUniform(&branch1, renUNIFTRANSY, 80);
    sceneInitialize(&root, &ren, unif, tex, &mesh1, &branch1, NULL);
    pixSetTimeStepHandler(handleTimeStep);
    pixRun();
  }
 }
