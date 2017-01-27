/*
 * 120mainCamera.c
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * main program to test camera position and angle changes
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

#define renUNIFDIM 41
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
#define renUNIFVIEWINGMAT 25

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
#include "120renderer.c"
#include "110triangle.c"
#include "100mesh.c"
#include "090scene.c"


double unif[renUNIFDIM] = {0};
renRenderer ren;
int width = 512;
int height = 512;
double target[3];
double lookatRho, lookatPhi, lookatTheta;
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
        mat44Isometry(rotation, trans, (double(*)[4])(&unif[renUNIFMAT00]));
        mat44Copy(ren->viewing, (double(*)[4])(&unif[renUNIFVIEWINGMAT]));
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
      mat44Copy(ren->viewing, (double(*)[4])(&unif[renUNIFVIEWINGMAT]));
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
  rgbz[3] = vary[renVARYZ];
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[],
        double vary[]) {
    double temp[4], temp2[4], temp3[4][4];
    double expandedAttr[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 1};
    mat444Multiply((double(*)[4])(&unif[renUNIFVIEWINGMAT]), (double(*)[4])(&unif[renUNIFMAT00]), temp3);
    mat441Multiply(temp3, expandedAttr, temp);
    vary[renATTRX] = temp[0];
    vary[renATTRY] = temp[1];
    vary[renATTRZ] = temp[2];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}


void handleTimeStep(double oldTime, double newTime) {
  if (floor(newTime) - floor(oldTime) >= 1.0) {
    printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
  }
  renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
  renUpdateViewing(&ren);
  pixClearRGB(0, 0, 0);
  depthClearZs(ren.depth, -99999);
  sceneSetUniform(&root, &ren, unif);
  sceneRender(&root, &ren, NULL);
}


/* The arrow keys control xy look at point, and the wasd keys controll the rotation.
    the key mappings are a bit unintuitive for now.*/
void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown) {
	printf("key up %d, shift %d, control %d, altOpt %d, supComm %d\n",
		key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
  switch (key) {
    case 87: { // w
      lookatPhi += .1;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }
    case 83: { // s
      lookatPhi -= .1;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }
    case 65: { // a
      lookatTheta -= .1;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }
    case 68: { // d
      lookatTheta -= .1;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }
    case 262: { // rightarrow
      target[0] = target[0] + 10;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }
    case 263: { // leftarrow
      target[0] = target[0] - 10;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }
    case 264: { // downarrow
      target[1] = target[1] - 10;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }
    case 265: { // uparrow
      target[1] = target[1] + 10;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }



  }

}

int main() {
  if (pixInitialize(width, height, "Pixel Graphics") != 0) {
    return 1;
  } else {
    pixClearRGB(0, 0, 0);
    depthBuffer dBuffer;
    depthInitialize(&dBuffer, width, height);

    // initialize ren
    ren.depth = &dBuffer;
    ren.attrDim = 8;
    ren.texNum = 1;
    ren.unifDim = renUNIFDIM;
    ren.varyDim = renVARYDIM;
    ren.transformVertex = transformVertex;
    ren.colorPixel = colorPixel;
    ren.updateUniform = updateUniform;
    target[0] = 0.0;
    target[1] = 0.0;
    target[2] = 0.0;
    lookatRho = 0.0;
    lookatPhi = 0.0;
    lookatTheta = 0.0;
    renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);

    // initialize textures`
    texTexture tex1;
    char *path1 = "cat1.jpg";
    texInitializeFile(&tex1, path1);
    texSetFiltering(&tex1, texQUADRATIC);
    texSetTopBottom(&tex1, texREPEAT);
    texSetLeftRight(&tex1, texREPEAT);
    tex[0] = &tex1;

    unif[renUNIFTRANSX]  = 0;
    unif[renUNIFTRANSY] = 0;
    unif[renUNIFTRANSZ] = 0;
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
    pixSetKeyUpHandler(handleKeyUp);

    pixRun();
  }
 }
