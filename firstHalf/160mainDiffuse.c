/*
 * 160mainDiffuse.c
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * main program to implement diffuse lighting
 */

#define renVARYDIMBOUND 40
#define renVERTNUMBOUND 200000

#define renATTRDIM 8
#define renATTRX 0
#define renATTRY 1
#define renATTRZ 2
#define renATTRS 3
#define renATTRT 4
#define renATTRN 5
#define renATTRO 6
#define renATTRP 7

#define renVARYDIM 15
#define renVARYX 0
#define renVARYY 1
#define renVARYZ 2
#define renVARYW 3
#define renVARYS 4
#define renVARYT 5
#define renVARYR 6
#define renVARYG 7
#define renVARYB 8
#define renVARYWORLDX 9
#define renVARYWORLDY 10
#define renVARYWORLDZ 11
#define renVARYWORLDNORMALX 12
#define renVARYWORLDNORMALY 13
#define renVARYWORLDNORMALZ 14

#define renUNIFDIM 47
#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renUNIFTRANSX 3
#define renUNIFTRANSY 4
#define renUNIFTRANSZ 5
#define renUNIFALPHA 6
#define renUNIFPHI 7
#define renUNIFTHETA 8
#define renUNIFM 9
#define renUNIFVIEWINGMAT 25
#define renUNIFLIGHTX 41
#define renUNIFLIGHTY 42
#define renUNIFLIGHTZ 43
#define renUNIFLIGHTR 44
#define renUNIFLIGHTG 45
#define renUNIFLIGHTB 46

#define texNUM 1
#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include "000pixel.h"
#include "100vector.c"
#include "130matrix.c"
#include "040texture.c"
#include "110depth.c"
#include "130renderer.c"
#include "110triangle.c"
#include "140clipping.c"
#include "140mesh.c"
#include "090scene.c"

double unif[renUNIFDIM] = {0};
renRenderer ren;
int width = 512;
int height = 512;
double target[3];
double lookatRho, lookatPhi, lookatTheta;
texTexture *tex[texNUM];
sceneNode root;

/* If unifParent is NULL, then sets the uniform matrix to the
rotation-translation M described by the other uniforms. If unifParent is not
NULL, but instead contains a rotation-translation P, then sets the uniform
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};
    double axis[3] = {0.0, 0.0, 1.0};
    double rotation[3][3];
    vec3Spherical(1.0, unif[renUNIFPHI], unif[renUNIFTHETA], axis);
    mat33AngleAxisRotation(unif[renUNIFALPHA], axis, rotation);
    // copy into the current mesh's unif
    mat44Copy(ren->viewing, (double(*)[4])(&unif[renUNIFVIEWINGMAT]));
    if (unifParent == NULL) {
        // make a rotation-translation matrix based on unifs
        mat44Isometry(rotation, trans, (double(*)[4])(&unif[renUNIFM]));
    } else {
        // make a rotation-translation matrix based on unifs
        double temp[4][4];
        mat44Isometry(rotation, trans, temp);
        // multiply with the existing rotation-translation matrix from the parent
        mat444Multiply((double(*)[4])(&unifParent[renUNIFM]),
                       temp,
                       (double(*)[4])(&unif[renUNIFM]));
    }

}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[],
        double vary[]) {
    double tempMat[4][4];
    double expandedAttr[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 1};
    double homog[4];
    mat444Multiply((double(*)[4])(&unif[renUNIFVIEWINGMAT]),
                   (double(*)[4])(&unif[renUNIFM]),
                    tempMat);
    mat441Multiply(tempMat, expandedAttr, homog);

    vary[renVARYX] = homog[0];
    vary[renVARYY] = homog[1];
    vary[renVARYZ] = homog[2];
    vary[renVARYW] = homog[3];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];

    // apply modeling transformations to the vertex points for lighting.
    double worldHomog[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 0};
    double worldNormalHomog[4] = {attr[renATTRN], attr[renATTRO], attr[renATTRP], 0};
    double worldResult[4], worldNormalResult[4], worldResultUnit[4], worldNormalResultUnit[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFM]), worldHomog, worldResult);
    mat441Multiply((double(*)[4])(&unif[renUNIFM]), worldNormalHomog, worldNormalResult);
    vecUnit(3, worldResult, worldResultUnit);
    vecUnit(3, worldNormalResult, worldNormalResultUnit);

    vary[renVARYWORLDX] = worldResultUnit[0];
    vary[renVARYWORLDY] = worldResultUnit[1];
    vary[renVARYWORLDZ] = worldResultUnit[2];
    vary[renVARYWORLDNORMALX] = worldNormalResultUnit[0];
    vary[renVARYWORLDNORMALY] = worldNormalResultUnit[1];
    vary[renVARYWORLDNORMALZ] = worldNormalResultUnit[2];
}

/* Sets rgb, based on the other parameters, which are unaltered. attr is an
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[],
                double vary[], double rgbz[]) {
  texSample(tex[0], vary[renVARYS], vary[renVARYT]);
  // double l[3] = {vary[renVARYWORLDX], vary[renVARYWORLDY], vary[renVARYWORLDZ]};
  double l[3] ={unif[renUNIFLIGHTX], unif[renUNIFLIGHTY], unif[renUNIFLIGHTZ]};
  double lUnit[3];
  vecUnit(3, l, lUnit);
  double n[3] = {vary[renVARYWORLDNORMALX], vary[renVARYWORLDNORMALY], vary[renVARYWORLDNORMALZ]};
  double diffuseIntensity;
  double dot = vecDot(3, lUnit, n);
  if (dot < 0) {
    diffuseIntensity = 0;
  } else {
    diffuseIntensity = dot;
  }
  //
  rgbz[0] = tex[0]->sample[renTEXR] * diffuseIntensity * unif[renUNIFLIGHTR];
  rgbz[1] = tex[0]->sample[renTEXG] * diffuseIntensity * unif[renUNIFLIGHTG];
  rgbz[2] = tex[0]->sample[renTEXB] * diffuseIntensity * unif[renUNIFLIGHTB];
  rgbz[3] = vary[renVARYZ];
}

void handleTimeStep(double oldTime, double newTime) {
  if (floor(newTime) - floor(oldTime) >= 1.0) {
    printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
  }
}

/* The arrow keys control xy look at point, and the wasd keys controll the rotation.
    the key mappings are a bit unintuitive for now.*/
void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown) {
	// printf("key up %d, shift %d, contrwidthol %d, altOpt %d, supComm %d\n",
	// 	key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
  switch (key) {
    case 49: {
      lookatRho -= 10;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }
    case 50: {
      lookatRho += 10;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }
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
      lookatTheta += .1;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }
    case 262: { // rightarrow
      target[0] = target[0] + 10;
      renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
      break;
    }
    case 263: { // leftarrow
      // target[1] = target[1] + 10;
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
  if (key) {
    renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
    // renSetFrustum(&ren, renORTHOGRAPHIC, M_PI / 6.0, 10.0, 10.0);
    renSetFrustum(&ren, renPERSPECTIVE, M_PI / 6.0, 10.0, 10.0);
    renUpdateViewing(&ren);
    pixClearRGB(0, 0, 0);
    depthClearZs(ren.depth, -999999999);
    sceneSetUniform(&root, &ren, unif);
    sceneRender(&root, &ren, NULL);
  }

}

int main() {
  if (pixInitialize(width, height, "Pixel Graphics") != 0) {
    return 1;
  } else {
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
    lookatRho = 1000.0;
    lookatPhi = 0.0;
    lookatTheta = 0.0;

    // modeling parameters
    unif[renUNIFTRANSX]  = 0;
    unif[renUNIFTRANSY] = 0;
    unif[renUNIFTRANSZ] = 0;
    unif[renUNIFALPHA] = 0;
    unif[renUNIFTHETA] = 0;
    unif[renUNIFPHI] = 0;
    // lighting parameters
    unif[renUNIFLIGHTX] = 100;
    unif[renUNIFLIGHTY] = 0;
    unif[renUNIFLIGHTZ] = 0;
    unif[renUNIFLIGHTR] = 1.0;
    unif[renUNIFLIGHTG] = 1.0;
    unif[renUNIFLIGHTB] = 1.0;

    renLookAt(&ren, target, lookatRho, lookatPhi, lookatTheta);
    renUpdateViewing(&ren);

    // initialize textures`
    texTexture tex1;
    char *path1 = "cat1.jpg";
    texInitializeFile(&tex1, path1);
    texSetFiltering(&tex1, texQUADRATIC);
    texSetTopBottom(&tex1, texREPEAT);
    texSetLeftRight(&tex1, texREPEAT);
    tex[0] = &tex1;

    // initialize some meshes
    meshMesh mesh1;
    meshInitializeSphere(&mesh1, 150, 20, 40);
    // meshInitializeBox(&mesh1, 0, 100, 0, 100, 0, 100);

    sceneInitialize(&root, &ren, unif, tex, &mesh1, NULL, NULL);
    pixSetTimeStepHandler(handleTimeStep);
    pixSetKeyUpHandler(handleKeyUp);
    pixRun();
  }
 }
