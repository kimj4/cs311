/*
 * 590mainShadowing.c
 * altered from existing code by Josh Davis
 * Ju Yun Kim
 * Carleton College
 * Late submission March 6, 2017
 * Added functionality for two light sources with shadow mapping.
 * One of the lights is red for ease of distinguishing.
 * in addition to the preexisting controls for the first light, use QWER to move
 * the second light around
 */

/* On macOS, compile with...
    clang 590mainShadowing.c /usr/local/gl3w/src/gl3w.o -lglfw -framework OpenGL -framework CoreFoundation
		On Ubuntu 16, compile with...
		clang 590mainShadowing.c /usr/local/gl3w/src/gl3w.o -lglfw3 -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lrt
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <sys/time.h>

double getTime(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec + (double)tv.tv_usec * 0.000001;
}

#include "500shader.c"
#include "530vector.c"
#include "580mesh.c"
#include "590matrix.c"
#include "520camera.c"
#include "590texture.c"
#include "580scene.c"
#include "560light.c"
#include "590shadow.c"

camCamera cam;
texTexture texH, texV, texW, texT, texL;
meshGLMesh meshH, meshV, meshW, meshT, meshL;
sceneNode nodeH, nodeV, nodeW, nodeT, nodeL;
/* We need just one shadow program, because all of our meshes have the same
attribute structure. */
shadowProgram sdwProg;
shadowProgram sdwProg2;
/* We need one shadow map per shadow-casting light. */
lightLight light;
shadowMap sdwMap;

lightLight light2;
shadowMap sdwMap2;
/* The main shader program has extra hooks for shadowing. */
GLuint program;
GLint viewingLoc, modelingLoc;
GLint unifLocs[1], textureLocs[1];
GLint attrLocs[3];
GLint lightPosLoc, lightColLoc, lightAttLoc, lightDirLoc, lightCosLoc;
GLint camPosLoc;
GLint viewingSdwLoc, textureSdwLoc;
GLint lightPosLoc2, lightColLoc2, lightAttLoc2, lightDirLoc2, lightCosLoc2;
GLint viewingSdwLoc2, textureSdwLoc2;

void handleError(int error, const char *description) {
	fprintf(stderr, "handleError: %d\n%s\n", error, description);
}

void handleResize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
	camSetWidthHeight(&cam, width, height);
}

void handleKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
	int shiftIsDown = mods & GLFW_MOD_SHIFT;
	int controlIsDown = mods & GLFW_MOD_CONTROL;
	int altOptionIsDown = mods & GLFW_MOD_ALT;
	int superCommandIsDown = mods & GLFW_MOD_SUPER;
	if (action == GLFW_PRESS && key == GLFW_KEY_L) {
		camSwitchProjectionType(&cam);
	} else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_O)
			camAddTheta(&cam, -0.1);
		else if (key == GLFW_KEY_P)
			camAddTheta(&cam, 0.1);
		else if (key == GLFW_KEY_I)
			camAddPhi(&cam, -0.1);
		else if (key == GLFW_KEY_K)
			camAddPhi(&cam, 0.1);
		else if (key == GLFW_KEY_U)
			camAddDistance(&cam, -0.5);
		else if (key == GLFW_KEY_J)
			camAddDistance(&cam, 0.5);
		else if (key == GLFW_KEY_Y) {
			GLdouble vec[3];
			vecCopy(3, light.translation, vec);
			vec[1] += 1.0;
			lightSetTranslation(&light, vec);
		} else if (key == GLFW_KEY_H) {
			GLdouble vec[3];
			vecCopy(3, light.translation, vec);
			vec[1] -= 1.0;
			lightSetTranslation(&light, vec);
		}
		else if (key == GLFW_KEY_T) {
			GLdouble vec[3];
			vecCopy(3, light.translation, vec);
			vec[0] += 1.0;
			lightSetTranslation(&light, vec);
		} else if (key == GLFW_KEY_G) {
			GLdouble vec[3];
			vecCopy(3, light.translation, vec);
			vec[0] -= 1.0;
			lightSetTranslation(&light, vec);
		} else if (key == GLFW_KEY_Q) {
			GLdouble vec[3];
			vecCopy(3, light2.translation, vec);
			vec[1] -= 1.0;
			lightSetTranslation(&light2, vec);
		} else if (key == GLFW_KEY_W) {
			GLdouble vec[3];
			vecCopy(3, light2.translation, vec);
			vec[0] += 1.0;
			lightSetTranslation(&light2, vec);
		} else if (key == GLFW_KEY_E) {
			GLdouble vec[3];
			vecCopy(3, light2.translation, vec);
			vec[0] -= 1.0;
			lightSetTranslation(&light2, vec);
		}else if (key == GLFW_KEY_R) {
			GLdouble vec[3];
			vecCopy(3, light2.translation, vec);
			vec[1] += 1.0;
			lightSetTranslation(&light2, vec);
		}
	}
}

/* Returns 0 on success, non-zero on failure. Warning: If initialization fails
midway through, then does not properly deallocate all resources. But that's
okay, because the program terminates almost immediately after this function
returns. */
int initializeScene(void) {
	if (texInitializeFile(&texH, "grass.jpg", GL_LINEAR, GL_LINEAR,
    		GL_REPEAT, GL_REPEAT) != 0)
    	return 1;
  if (texInitializeFile(&texV, "granite.jpg", GL_LINEAR, GL_LINEAR,
  		GL_REPEAT, GL_REPEAT) != 0)
  	return 2;
		printf("here\n");

  if (texInitializeFile(&texW, "water.jpg", GL_LINEAR, GL_LINEAR,
  		GL_REPEAT, GL_REPEAT) != 0)
  	return 3;
  if (texInitializeFile(&texT, "trunk.jpg", GL_LINEAR, GL_LINEAR,
  		GL_REPEAT, GL_REPEAT) != 0)
  	return 4;
  if (texInitializeFile(&texL, "tree.jpg", GL_LINEAR, GL_LINEAR,
  		GL_REPEAT, GL_REPEAT) != 0)
  	return 5;
	GLuint attrDims[3] = {3, 2, 3};
    double zs[12][12] = {
		{5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 20.0},
		{5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 20.0, 25.0},
		{5.0, 5.0, 10.0, 12.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 20.0, 25.0},
		{5.0, 5.0, 10.0, 10.0, 5.0, 5.0, 5.0, 5.0, 5.0, 20.0, 25.0, 27.0},
		{0.0, 0.0, 5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 20.0, 20.0, 25.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 20.0, 25.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 5.0, 7.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 20.0, 20.0},
		{5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 20.0, 20.0, 20.0},
		{10.0, 10.0, 5.0, 5.0, 0.0, 0.0, 0.0, 5.0, 10.0, 15.0, 20.0, 25.0}};
	double ws[12][12] = {
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}};
	meshMesh mesh, meshLand;
	if (meshInitializeLandscape(&meshLand, 12, 12, 5.0, (double *)zs) != 0)
		return 6;
	if (meshInitializeDissectedLandscape(&mesh, &meshLand, M_PI / 3.0, 1) != 0)
		return 7;
	/* There are now two VAOs per mesh. */
	// changes: there are 3 to accomodate 2 light sources
	meshGLInitialize(&meshH, &mesh, 3, attrDims, 3);
	meshGLVAOInitialize(&meshH, 0, attrLocs);
	meshGLVAOInitialize(&meshH, 1, sdwProg.attrLocs);
	meshGLVAOInitialize(&meshH, 2, sdwProg2.attrLocs);
	meshDestroy(&mesh);
	if (meshInitializeDissectedLandscape(&mesh, &meshLand, M_PI / 3.0, 0) != 0)
		return 8;
	meshDestroy(&meshLand);
	double *vert, normal[2];
	for (int i = 0; i < mesh.vertNum; i += 1) {
		vert = meshGetVertexPointer(&mesh, i);
		normal[0] = -vert[6];
		normal[1] = vert[5];
		vert[3] = (vert[0] * normal[0] + vert[1] * normal[1]) / 20.0;
		vert[4] = vert[2] / 20.0;
	}
	meshGLInitialize(&meshV, &mesh, 3, attrDims, 3);
	meshGLVAOInitialize(&meshV, 0, attrLocs);
	meshGLVAOInitialize(&meshV, 1, sdwProg.attrLocs);
	meshGLVAOInitialize(&meshV, 2, sdwProg2.attrLocs);
	meshDestroy(&mesh);
	if (meshInitializeLandscape(&mesh, 12, 12, 5.0, (double *)ws) != 0)
		return 9;
	meshGLInitialize(&meshW, &mesh, 3, attrDims, 3);
	meshGLVAOInitialize(&meshW, 0, attrLocs);
	meshGLVAOInitialize(&meshW, 1, sdwProg.attrLocs);
	meshGLVAOInitialize(&meshW, 2, sdwProg2.attrLocs);
	meshDestroy(&mesh);
	if (meshInitializeCapsule(&mesh, 1.0, 10.0, 1, 8) != 0)
		return 10;
	meshGLInitialize(&meshT, &mesh, 3, attrDims, 3);
	meshGLVAOInitialize(&meshT, 0, attrLocs);
	meshGLVAOInitialize(&meshT, 1, sdwProg.attrLocs);
	meshGLVAOInitialize(&meshT, 2, sdwProg2.attrLocs);
	meshDestroy(&mesh);
	if (meshInitializeSphere(&mesh, 5.0, 8, 16) != 0)
		return 11;
	meshGLInitialize(&meshL, &mesh, 3, attrDims, 3);
	meshGLVAOInitialize(&meshL, 0, attrLocs);
	meshGLVAOInitialize(&meshL, 1, sdwProg.attrLocs);
	meshGLVAOInitialize(&meshL, 2, sdwProg2.attrLocs);
	meshDestroy(&mesh);
	if (sceneInitialize(&nodeW, 3, 1, &meshW, NULL, NULL) != 0)
		return 14;
	if (sceneInitialize(&nodeL, 3, 1, &meshL, NULL, NULL) != 0)
		return 16;
	if (sceneInitialize(&nodeT, 3, 1, &meshT, &nodeL, &nodeW) != 0)
		return 15;
	if (sceneInitialize(&nodeV, 3, 1, &meshV, NULL, &nodeT) != 0)
		return 13;
	if (sceneInitialize(&nodeH, 3, 1, &meshH, &nodeV, NULL) != 0)
		return 12;
	GLdouble trans[3] = {40.0, 28.0, 5.0};
	sceneSetTranslation(&nodeT, trans);
	vecSet(3, trans, 0.0, 0.0, 7.0);
	sceneSetTranslation(&nodeL, trans);
	GLdouble unif[3] = {0.0, 0.0, 0.0};
	sceneSetUniform(&nodeH, unif);
	sceneSetUniform(&nodeV, unif);
	sceneSetUniform(&nodeT, unif);
	sceneSetUniform(&nodeL, unif);
	vecSet(3, unif, 1.0, 1.0, 1.0);
	sceneSetUniform(&nodeW, unif);
	texTexture *tex;
	tex = &texH;
	sceneSetTexture(&nodeH, &tex);
	tex = &texV;
	sceneSetTexture(&nodeV, &tex);
	tex = &texW;
	sceneSetTexture(&nodeW, &tex);
	tex = &texT;
	sceneSetTexture(&nodeT, &tex);
	tex = &texL;
	sceneSetTexture(&nodeL, &tex);
	return 0;
}

void destroyScene(void) {
	texDestroy(&texH);
	texDestroy(&texV);
	texDestroy(&texW);
	texDestroy(&texT);
	texDestroy(&texL);
	meshGLDestroy(&meshH);
	meshGLDestroy(&meshV);
	meshGLDestroy(&meshW);
	meshGLDestroy(&meshT);
	meshGLDestroy(&meshL);
	sceneDestroyRecursively(&nodeH);
}

/* Returns 0 on success, non-zero on failure. Warning: If initialization fails
midway through, then does not properly deallocate all resources. But that's
okay, because the program terminates almost immediately after this function
returns. */
int initializeCameraLight(void) {
  GLdouble vec[3] = {30.0, 30.0, 5.0};

	// for first light
	camSetControls(&cam, camPERSPECTIVE, M_PI / 6.0, 10.0, 768.0, 768.0, 100.0,
								 M_PI / 4.0, M_PI / 4.0, vec);
	lightSetType(&light, lightSPOT);
	vecSet(3, vec, 45.0, 30.0, 20.0);
	lightShineFrom(&light, vec, M_PI * 3.0 / 4.0, M_PI * 3.0 / 4.0);
	vecSet(3, vec, 1.0, 1.0, 1.0);
	lightSetColor(&light, vec);
	vecSet(3, vec, 1.0, 0.0, 0.0);
	lightSetAttenuation(&light, vec);
	lightSetSpotAngle(&light, M_PI / 3.0);
	/* Configure shadow mapping. */
	if (shadowProgramInitialize(&sdwProg, 3) != 0)
		return 1;
	if (shadowMapInitialize(&sdwMap, 1024, 1024) != 0)
		return 2;

	// for second light
	vecSet(3, vec, 40.0, 35.0, 5.0);
	camSetControls(&cam, camPERSPECTIVE, M_PI / 6.0, 10.0, 768.0, 768.0, 100.0,
								 M_PI / 4.0, M_PI / 4.0, vec);
	lightSetType(&light2, lightSPOT);
	vecSet(3, vec, 50.0, 40.0, 15.0);
	lightShineFrom(&light2, vec, M_PI * 3.0 / 4.0, M_PI * 3.0 / 4.0);
	vecSet(3, vec, 1.0, 0.0, 0.0); // red light to distinguish itself
	lightSetColor(&light2, vec);
	vecSet(3, vec, 1.0, 0.0, 0.0);
	lightSetAttenuation(&light2, vec);
	lightSetSpotAngle(&light2, M_PI / 3.0);
	if (shadowProgramInitialize(&sdwProg2, 3) != 0)
		return 3;
	if (shadowMapInitialize(&sdwMap2, 1024, 1024) != 0)
		return 4;
	return 0;
}

/* Returns 0 on success, non-zero on failure. */
int initializeShaderProgram(void) {
	GLchar vertexCode[] = "\
		#version 140\n\
		uniform mat4 viewing;\
		uniform mat4 modeling;\
		uniform mat4 viewingSdw;\
		uniform mat4 viewingSdw2;\
		in vec3 position;\
		in vec2 texCoords;\
		in vec3 normal;\
		out vec3 fragPos;\
		out vec3 normalDir;\
		out vec2 st;\
		out vec4 fragSdw;\
		out vec4 fragSdw2;\
		void main(void) {\
			mat4 scaleBias = mat4(\
				0.5, 0.0, 0.0, 0.0, \
				0.0, 0.5, 0.0, 0.0, \
				0.0, 0.0, 0.5, 0.0, \
				0.5, 0.5, 0.5, 1.0);\
			vec4 worldPos = modeling * vec4(position, 1.0);\
			gl_Position = viewing * worldPos;\
			fragSdw = scaleBias * viewingSdw * worldPos;\
			fragSdw2 = scaleBias * viewingSdw2 * worldPos;\
			fragPos = vec3(worldPos);\
			normalDir = vec3(modeling * vec4(normal, 0.0));\
			st = texCoords;\
		}";
	GLchar fragmentCode[] = "\
		#version 140\n\
		uniform sampler2D texture0;\
		uniform vec3 specular;\
		uniform vec3 camPos;\
		uniform vec3 lightPos;\
		uniform vec3 lightCol;\
		uniform vec3 lightAtt;\
		uniform vec3 lightAim;\
		uniform float lightCos;\
		uniform vec3 lightPos2;\
		uniform vec3 lightCol2;\
		uniform vec3 lightAtt2;\
		uniform vec3 lightAim2;\
		uniform float lightCos2;\
		uniform sampler2DShadow textureSdw;\
		uniform sampler2DShadow textureSdw2;\
		in vec3 fragPos;\
		in vec3 normalDir;\
		in vec2 st;\
		in vec4 fragSdw;\
		in vec4 fragSdw2;\
		out vec4 fragColor;\
		void main(void) {\
			vec3 diffuse = vec3(texture(texture0, st));\
			vec3 diffuse2 = vec3(texture(texture0, st));\
			vec3 litDir = normalize(lightPos - fragPos);\
			vec3 litDir2 = normalize(lightPos2 - fragPos);\
			float diffInt, specInt = 0.0;\
			float diffInt2, specInt2 = 0.0;\
			if (dot(lightAim, -litDir) < lightCos)\
				diffInt = 0.0;\
			else\
				diffInt = 1.0;\
			if (dot(lightAim2, -litDir2) < lightCos2)\
				diffInt2 = 0.0;\
			else\
				diffInt2 = 1.0;\
			float sdw = textureProj(textureSdw, fragSdw);\
			diffInt *= sdw;\
			specInt *= sdw;\
			float sdw2 = textureProj(textureSdw2, fragSdw2);\
			diffInt *= sdw2;\
			specInt *= sdw2;\
			vec3 diffRefl = max(0.2, diffInt) * lightCol * diffuse;\
			vec3 diffRefl2 = max(0.2, diffInt2) * lightCol2 * diffuse2;\
			vec3 specRefl = specInt * lightCol * specular;\
			vec3 specRefl2 = specInt2 * lightCol2 * specular;\
			fragColor = vec4(diffRefl + specRefl + diffRefl2 + specRefl2, 1.0);\
		}";
	program = makeProgram(vertexCode, fragmentCode);
	if (program != 0) {
		glUseProgram(program);
		attrLocs[0] = glGetAttribLocation(program, "position");
		attrLocs[1] = glGetAttribLocation(program, "texCoords");
		attrLocs[2] = glGetAttribLocation(program, "normal");
		viewingLoc = glGetUniformLocation(program, "viewing");
		modelingLoc = glGetUniformLocation(program, "modeling");
		unifLocs[0] = glGetUniformLocation(program, "specular");
		textureLocs[0] = glGetUniformLocation(program, "texture0");
		camPosLoc = glGetUniformLocation(program, "camPos");
		lightPosLoc = glGetUniformLocation(program, "lightPos");
		lightColLoc = glGetUniformLocation(program, "lightCol");
		lightAttLoc = glGetUniformLocation(program, "lightAtt");
		lightDirLoc = glGetUniformLocation(program, "lightAim");
		lightCosLoc = glGetUniformLocation(program, "lightCos");
		lightPosLoc2 = glGetUniformLocation(program, "lightPos2");
		lightColLoc2 = glGetUniformLocation(program, "lightCol2");
		lightAttLoc2 = glGetUniformLocation(program, "lightAtt2");
		lightDirLoc2 = glGetUniformLocation(program, "lightAim2");
		lightCosLoc2 = glGetUniformLocation(program, "lightCos2");
		viewingSdwLoc = glGetUniformLocation(program, "viewingSdw");
		viewingSdwLoc2 = glGetUniformLocation(program, "viewingSdw2");
		textureSdwLoc = glGetUniformLocation(program, "textureSdw");
		textureSdwLoc2 = glGetUniformLocation(program, "textureSdw2");
	}

	return (program == 0);
}

void render(void) {
	GLdouble identity[4][4];
	mat44Identity(identity);

	/* Save the viewport transformation. */
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLint sdwTextureLocs[1] = {-1};
	GLint sdwTextureLocs2[1] = {-1};

	// first pass
	shadowMapRender(&sdwMap, &sdwProg, &light, -100.0, -1.0);
	sceneRender(&nodeH, identity, sdwProg.modelingLoc, 0, NULL, NULL, 1,
							sdwTextureLocs);

	// second pass
	shadowMapRender(&sdwMap2, &sdwProg2, &light2, -100.0, -1.0);
	sceneRender(&nodeH, identity, sdwProg2.modelingLoc, 0, NULL, NULL, 1,
							sdwTextureLocs2);

	shadowMapUnrender();

	/* Finish preparing the shadow maps, restore the viewport, and begin to
	render the scene. */

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	/* For each light, we have to connect it to the shader program, as always.
	For each shadow-casting light, we must also connect its shadow map. */
	camRender(&cam, viewingLoc);
	GLfloat vec[3];
	vecOpenGL(3, cam.translation, vec);
	glUniform3fv(camPosLoc, 1, vec);
	lightRender(&light, lightPosLoc, lightColLoc, lightAttLoc, lightDirLoc,
							lightCosLoc);
	shadowRender(&sdwMap, viewingSdwLoc, GL_TEXTURE7, 7, textureSdwLoc);

	lightRender(&light2, lightPosLoc2, lightColLoc2, lightAttLoc2, lightDirLoc2,
							lightCosLoc2);
	shadowRender(&sdwMap2, viewingSdwLoc2, GL_TEXTURE6, 6, textureSdwLoc2);
	GLuint unifDims[1] = {3};
	sceneRender(&nodeH, identity, modelingLoc, 1, unifDims, unifLocs, 0,
							textureLocs);

	/* For each shadow-casting light, turn it off when finished rendering. */
	shadowUnrender(GL_TEXTURE7);
	shadowUnrender(GL_TEXTURE6);
}

int main(void) {
	double oldTime;
	double newTime = getTime();
  glfwSetErrorCallback(handleError);
  if (glfwInit() == 0) {
  	fprintf(stderr, "main: glfwInit failed.\n");
    return 1;
  }
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  GLFWwindow *window;
  window = glfwCreateWindow(768, 768, "Shadows", NULL, NULL);
  if (window == NULL) {
  	fprintf(stderr, "main: glfwCreateWindow failed.\n");
    glfwTerminate();
    return 2;
  }
  glfwSetWindowSizeCallback(window, handleResize);
  glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, handleKey);
  if (gl3wInit() != 0) {
  	fprintf(stderr, "main: gl3wInit failed.\n");
  	glfwDestroyWindow(window);
  	glfwTerminate();
  	return 3;
  }

  fprintf(stderr, "main: OpenGL %s, GLSL %s.\n",
					glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	/* We no longer do glDepthRange(1.0, 0.0). Instead we have changed our
	projection matrices. */
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  if (initializeShaderProgram() != 0)
  	return 3;
  /* Initialize the shadow mapping before the meshes. Why? */
	if (initializeCameraLight() != 0)
		return 4;
  if (initializeScene() != 0)
  	return 5;

  while (glfwWindowShouldClose(window) == 0) {
  	oldTime = newTime;
  	newTime = getTime();
  	if (floor(newTime) - floor(oldTime) >= 1.0)
		fprintf(stderr, "main: %f frames/sec\n", 1.0 / (newTime - oldTime));
		render();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  /* Deallocate more resources than ever. */
  shadowProgramDestroy(&sdwProg);
	shadowProgramDestroy(&sdwProg2);
  shadowMapDestroy(&sdwMap);
	shadowMapDestroy(&sdwMap2);
  glDeleteProgram(program);
  destroyScene();
	glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
