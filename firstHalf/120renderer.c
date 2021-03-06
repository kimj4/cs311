/*
 * 120renderer.c
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * now with some functions for the camera
 */
typedef struct renRenderer renRenderer;

struct renRenderer {
	int unifDim; // dimension of stuff properties that apply to the whole triangle
	int texNum;  // number of textures
	int attrDim; // dimension of information specific to each vertex
  int varyDim;
  depthBuffer *depth;
  double cameraRotation[3][3];
  double cameraTranslation[3];
  double viewing[4][4];
	// function pointers
  void (*colorPixel)(renRenderer *, double[], texTexture *[], double[], double[]);
  void (*transformVertex)(renRenderer *, double[], double[], double[]);
	void (*updateUniform)(renRenderer *, double[], double[]);
};
/* Sets the camera's rotation and translation, in a manner suitable for third-
person viewing. The camera is aimed at the world coordinates target. The camera
itself is displaced from that target by a distance rho, in the direction
specified by the spherical coordinates phi and theta (as in vec3Spherical).
Under normal use the camera's up-direction is world-up, or as close to it as
possible. */
void renLookAt(renRenderer *ren, double target[3], double rho, double phi,
        double theta) {
    double z[3], y[3], yStd[3] = {0.0, 1.0, 0.0}, zStd[3] = {0.0, 0.0, 1.0};
    vec3Spherical(1.0, phi, theta, z);
    vec3Spherical(1.0, M_PI / 2.0 - phi, theta + M_PI, y);
    mat33BasisRotation(yStd, zStd, y, z, ren->cameraRotation);
    vecScale(3, rho, z, ren->cameraTranslation);
    vecAdd(3, target, ren->cameraTranslation, ren->cameraTranslation);
}

/* Sets the camera's rotation and translation, in a manner suitable for first-
person viewing. The camera is positioned at the world coordinates position.
From that position, the camera's sight direction is described by the spherical
coordinates phi and theta (as in vec3Spherical). Under normal use the camera's
up-direction is world-up, or as close to it as possible. */
void renLookFrom(renRenderer *ren, double position[3], double phi,
        double theta) {
    double negZ[3], y[3];
    double yStd[3] = {0.0, 1.0, 0.0}, negZStd[3] = {0.0, 0.0, -1.0};
    vec3Spherical(1.0, phi, theta, negZ);
    vec3Spherical(1.0, M_PI / 2.0 - phi, theta + M_PI, y);
    mat33BasisRotation(yStd, negZStd, y, negZ, ren->cameraRotation);
    vecCopy(3, position, ren->cameraTranslation);
}

/* Updates the renderer's viewing transformation, based on the camera. */
void renUpdateViewing(renRenderer *ren) {
    mat44InverseIsometry(ren->cameraRotation, ren->cameraTranslation, ren->viewing);
  }
