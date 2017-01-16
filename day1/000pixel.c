


/*
On macOS, compile with...
	clang -c 000pixel.c
...and then link with a main program by...
	clang main.c 000pixel.o
*/



/*** Private: infrastructure ***/

#include <stdio.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
//#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <sys/time.h>
//#include <time.h>

// Global variables.
GLFWwindow *pixWindow;
int pixWidth, pixHeight;
GLuint pixTexture;
GLfloat *pixPixels;
int pixNeedsRedisplay = 1;
GLuint pixAttrBuffer, pixTriBuffer;
GLuint pixProgram;
GLint pixUnifLoc, pixAttrLoc;
double pixOldTime, pixNewTime;
void (*pixUserKeyDownHandler)(int, int, int, int, int) = NULL;
void (*pixUserKeyUpHandler)(int, int, int, int, int) = NULL;
void (*pixUserKeyRepeatHandler)(int, int, int, int, int) = NULL;
void (*pixUserMouseDownHandler)(int, int, int, int, int) = NULL;
void (*pixUserMouseUpHandler)(int, int, int, int, int) = NULL;
void (*pixUserMouseMoveHandler)(double, double) = NULL;
void (*pixUserMouseScrollHandler)(double, double) = NULL;
void (*pixUserTimeStepHandler)(double, double) = NULL;

int pixPowerOfTwoFloor(int n) {
	int m = 1;
	while (m <= n)
		m *= 2;
	return m / 2;
}

double pixTime(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec + (double)tv.tv_usec * 0.000001;
}

GLuint pixBuildVertexFragmentProgram(const GLchar *vertexCode, 
		const GLchar *fragmentCode) {
	GLuint vertexShader, fragmentShader, program;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexCode, 0);
	// !!check for errors from glCompileShader
	glCompileShader(vertexShader);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentCode, 0);
	// !!check for errors from glCompileShader
	glCompileShader(fragmentShader);
	program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    return program;
}



/*** Private: GLFW handlers ***/

void pixHandleError(int error, const char *description) {
	fprintf(stderr, "error: pixHandleError: GLFW code %d, message...\n%s\n",
		error, description);
}

// key is GLFW_KEY_A, GLFW_KEY_B, etc. Or GLFW_KEY_UNKNOWN.
// scancode is a platform-dependent scan code for the key.
// action is GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT.
// mods has bitwise masks for...
//     GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, or GLFW_MOD_SUPER
// ...which on macOS mean shift, control, option, command.
void pixHandleKey(GLFWwindow *window, int key, int scancode, int action,
		int mods) {
	int shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown;
	shiftIsDown = mods & GLFW_MOD_SHIFT;
	controlIsDown = mods & GLFW_MOD_CONTROL;
	altOptionIsDown = mods & GLFW_MOD_ALT;
	superCommandIsDown = mods & GLFW_MOD_SUPER;
	if (action == GLFW_PRESS && pixUserKeyDownHandler != NULL)
		pixUserKeyDownHandler(key, shiftIsDown, controlIsDown, altOptionIsDown, 
			superCommandIsDown);
	else if (action == GLFW_RELEASE && pixUserKeyUpHandler != NULL)
		pixUserKeyUpHandler(key, shiftIsDown, controlIsDown, altOptionIsDown, 
			superCommandIsDown);
	else if (action == GLFW_REPEAT && pixUserKeyRepeatHandler != NULL)
		pixUserKeyRepeatHandler(key, shiftIsDown, controlIsDown, 
			altOptionIsDown, superCommandIsDown);
}

// button is GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, or...?
// action is GLFW_PRESS or GLFW_RELEASE
// mods has bitwise masks for...
//     GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, or GLFW_MOD_SUPER
// ...which on macOS mean shift, control, option, command.
void pixHandleMouseButton(GLFWwindow *window, int button, int action, 
		int mods) {
	int shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown;
	shiftIsDown = mods & GLFW_MOD_SHIFT;
	controlIsDown = mods & GLFW_MOD_CONTROL;
	altOptionIsDown = mods & GLFW_MOD_ALT;
	superCommandIsDown = mods & GLFW_MOD_SUPER;
	if (action == GLFW_PRESS && pixUserMouseDownHandler != NULL)
		pixUserMouseDownHandler(button, shiftIsDown, controlIsDown, 
			altOptionIsDown, superCommandIsDown);
	else if (action == GLFW_RELEASE && pixUserMouseUpHandler != NULL)
		pixUserMouseUpHandler(button, shiftIsDown, controlIsDown, 
			altOptionIsDown, superCommandIsDown);
}

// The origin is in the upper left corner of the window.
void pixHandleMouseMove(GLFWwindow *window, double x, double y) {
	if (pixUserMouseMoveHandler != NULL) {
		// Flip vertically, so that the origin is in the lower left.
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pixUserMouseMoveHandler(x, height - y);
	}
}

// The origin is in the upper left corner of the window.
// A 1D scroll wheel will report 0.0 for xOffset.
void pixHandleMouseScroll(GLFWwindow *window, double xOffset, double yOffset) {
	if (pixUserMouseScrollHandler != NULL)
		// Flip vertically, so that the origin is in the lower left.
		pixUserMouseScrollHandler(xOffset, -yOffset);
}



/*** Private: initializer ***/

// Initialize the user interface and OpenGL.
int pixInitGLFWGL3W(char *name) {
	glfwSetErrorCallback(pixHandleError);
	// !!error if glfwInit() returns GL_FALSE
	glfwInit();
	// This code is commented-out because my machine doesn't go above 2.1.
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	pixWindow = glfwCreateWindow(pixWidth, pixHeight, name, NULL, NULL);
	glfwMakeContextCurrent(pixWindow);
	fprintf(stderr, "pixInitialize: OpenGL %s, GLSL %s.\n", 
		glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	glfwSetKeyCallback(pixWindow, pixHandleKey);
	glfwSetCursorPosCallback(pixWindow, pixHandleMouseMove);
	glfwSetMouseButtonCallback(pixWindow, pixHandleMouseButton);
	glfwSetScrollCallback(pixWindow, pixHandleMouseScroll);
	// !!error if gl3wInit returns anything but 0
	//gl3wInit();
	glViewport(0, 0, pixWidth, pixHeight);
	return 0;
}

// Create the texture.
int pixInitTexture() {
	// !!error if malloc returns NULL
	pixPixels = (GLfloat *)malloc(3 * pixWidth * pixHeight * sizeof(GLfloat));
	// If we were using OpenGL 4.5, we would do this.
	//glCreateTextures(GL_TEXTURE_RECTANGLE, 1, &pixTexture);
	//glTextureStorage2D(pixTexture, 1, GL_RGB32F, pixWidth, pixHeight);
	//glBindTextureUnit(0, pixTexture);
	// But in OpenGL 2.1 we do this instead.
	glGenTextures(1, &pixTexture);
	glBindTexture(GL_TEXTURE_2D, pixTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixWidth, pixHeight, 0, GL_RGB, 
		GL_FLOAT, pixPixels);
	// !!error if glGetError returns anything but GL_NO_ERROR
	//glGetError() != GL_NO_ERROR
	return 0;
}

/*
// Vertex shader.
uniform mat4 cameraMatrix;
attribute vec4 attribs;
varying vec2 texCoords;
void main(){
	gl_Position = cameraMatrix * vec4(attribs[0], attribs[1], 0.0, 1.0);
	texCoords = vec2(attribs[2], attribs[3]);
}
// Fragment shader.
varying vec2 texCoords;
uniform sampler2D texture0;
void main(){
	gl_FragColor = texture2D(texture0, texCoords);
}
*/
// Build the shader program and leave it bound.
int pixInitShaders() {
	GLchar vertexCode[] = "uniform mat4 cameraMatrix;attribute vec4 attribs;varying vec2 texCoords;void main(){gl_Position = cameraMatrix * vec4(attribs[0], attribs[1], 0.0, 1.0);texCoords = vec2(attribs[2], attribs[3]);}";
	GLchar fragmentCode[] = "varying vec2 texCoords;uniform sampler2D texture0;void main(){gl_FragColor = texture2D(texture0, texCoords);}";
	pixProgram = pixBuildVertexFragmentProgram(vertexCode, fragmentCode);
    glUseProgram(pixProgram);
	// Load identifiers for sampler units, for some reason before validating.
    glUniform1i(glGetUniformLocation(pixProgram, "texture0"), 0);
	//!!validateShaderProgram(self.drawingProgram)
    pixUnifLoc = glGetUniformLocation(pixProgram, "cameraMatrix");
    pixAttrLoc = glGetAttribLocation(pixProgram, "attribs");
	return 0;
}

// Make a rectangle from two triangles. Leave the OpenGL buffers bound.
int pixInitMesh() {
	GLfloat attributes[4 * 4] = {
		0.0, 0.0, 0.0, 0.0,
		pixWidth, 0.0, 1.0, 0.0, 
		pixWidth, pixHeight, 1.0, 1.0,
		0.0, pixHeight, 0.0, 1.0};
	GLushort triangles[2 * 3] = {0, 1, 2, 0, 2, 3};
	glGenBuffers(1, &pixAttrBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pixAttrBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(GLfloat),
		(GLvoid *)attributes, GL_STATIC_DRAW);
	glGenBuffers(1, &pixTriBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pixTriBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(GLushort),
		(GLvoid *)triangles, GL_STATIC_DRAW);
	glVertexAttribPointer(pixAttrLoc, 4, GL_FLOAT, 0, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(pixAttrLoc);
	return 0;
}



/*** Public: miscellaneous ***/

/* A pixel system program usually proceeds through these five steps:
	A. pixInitialize is invoked and terminates.
	B. Some pixel system functions, other than pixInitialize and pixRun, are 
		invoked.
	C. pixRun is invoked.
	D. Some pixel system functions, other than pixInitialize and pixRun, are 
		invoked, automatically by user event handlers.
	E. The user elects to quit the program, thus causing pixRun to terminate.
Sometimes a pixel system program makes multiple loops through these five steps. 
The important thing is that invocations of pixInitialize and pixRun come in 
non-overlapping pairs. The pixel system is designed to support only one active 
window at a time. */

/* Initializes the pixel system. This function must be called before any other 
pixel system functions. The width and height parameters describe the size of 
the window. They should be powers of 2. The name parameter is a string for the 
window's title. Returns an error code, which is 0 if no error occurred. */
int pixInitialize(int width, int height, char *name) {
	int error = 0;
	pixWidth = pixPowerOfTwoFloor(width);
	pixHeight = pixPowerOfTwoFloor(height);
	if (pixWidth != width || pixHeight != height) {
		fprintf(stderr, "warning: pixInitialize: ");
		fprintf(stderr, "forcing width, height to be powers of 2.\n");
	}
	// !!when errors happen midway through, should deallocate resources so far
	error = pixInitGLFWGL3W(name);
	if (!error)
		error = pixInitTexture();
	if (!error)
		error = pixInitShaders();
	if (!error)
		error = pixInitMesh();
	pixNewTime = pixTime();
	return 0;
}

/* Runs the event loop. First, any pending user events are processed by their 
corresponding callbacks. Second, the time step callback is invoked. Third, if 
any drawing has occurred, then the screen is updated to reflect that drawing. 
When the user elects to quit, the resources supporting the window are 
deallocated, and this function terminates; pixInitialize must be called again, 
before any further use of the pixel system. */
void pixRun(void) {
	// Repeatedly process user input and redraw if necessary.
	while (glfwWindowShouldClose(pixWindow) == GL_FALSE) {
		glfwPollEvents();
		pixOldTime = pixNewTime;
		pixNewTime = pixTime();
		if (pixUserTimeStepHandler != NULL)
			pixUserTimeStepHandler(pixOldTime, pixNewTime);
		if (pixNeedsRedisplay) {
			// In OpenGL 4.5 we would do this.
			//glTextureSubImage2D(pixTexture, 0, 0, 0, pixWidth, pixHeight, 
			//	GL_RGB, GL_FLOAT, pixPixels);
			// But in OpenGL 2.1 we do this.
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pixWidth, pixHeight, 
				GL_RGB, GL_FLOAT, pixPixels);
			GLfloat matrix[] = {
				2.0 / pixWidth, 0.0, 0.0, -1.0,
				0.0, 2.0 / pixHeight, 0.0, -1.0,
				0.0, 0.0, -1.0, 0.0,
				0.0, 0.0, 0.0, 1.0};
			glUniformMatrix4fv(pixUnifLoc, 1, GL_TRUE, matrix);
			glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_SHORT, 0);
			glfwSwapBuffers(pixWindow);
			pixNeedsRedisplay = 0;
		}
	}
	// Clean up the allocated resources.
	glDisableVertexAttribArray(pixAttrLoc);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &pixTriBuffer);
	glDeleteBuffers(1, &pixAttrBuffer);
	//glUseProgram(0);
	glDeleteProgram(pixProgram);
	glDeleteTextures(1, &pixTexture);
	free(pixPixels);
	glfwDestroyWindow(pixWindow);
	glfwTerminate();
}

/* Returns the red channel of the pixel at coordinates (x, y). Coordinates are 
relative to the lower left corner of the window. */
double pixGetR(int x, int y) {
	if (0 <= x && x < pixWidth && 0 <= y && y < pixHeight)
		return pixPixels[3 * (x + pixWidth * y)];
	else
		return -1.0;
}

/* Returns the green channel of the pixel at coordinates (x, y). Coordinates 
are relative to the lower left corner of the window. */
double pixGetG(int x, int y) {
	if (0 <= x && x < pixWidth && 0 <= y && y < pixHeight)
		return pixPixels[3 * (x + pixWidth * y) + 1];
	else
		return -1.0;
}

/* Returns the blue channel of the pixel at coordinates (x, y). Coordinates are 
relative to the lower left corner of the window. */
double pixGetB(int x, int y) {
	if (0 <= x && x < pixWidth && 0 <= y && y < pixHeight)
		return pixPixels[3 * (x + pixWidth * y) + 2];
	else
		return -1.0;
}

/* Sets the pixel at coordinates (x, y) to the given RGB color. Coordinates are 
relative to the lower left corner of the window. */
void pixSetRGB(int x, int y, double red, double green, double blue) {
	if (0 <= x && x < pixWidth && 0 <= y && y < pixHeight) {
		int index = 3 * (x + pixWidth * y);
		pixPixels[index] = red;
		pixPixels[index + 1] = green;
		pixPixels[index + 2] = blue;
		pixNeedsRedisplay = 1;
	}
}

/* Sets all pixels to the given RGB color. */
void pixClearRGB(double red, double green, double blue) {
	int index, bound;
	bound = 3 * pixWidth * pixHeight;
	for (index = 0; index < bound; index += 3) {
		pixPixels[index] = red;
		pixPixels[index + 1] = green;
		pixPixels[index + 2] = blue;
	}
	pixNeedsRedisplay = 1;
}

/* Copies pixel data from a rectangle region of the screen into the given 
buffer. The rectangular region has lower left corner [x y]^T and the given 
width and height. The pixels are copied by row, starting in the lower left 
corner of the region. The buffer must be pre-allocated to hold 
width * height * 3 doubles. There's not much error checking. */
/* Seems buggy.
void pixCopyPixels(int x, int y, int width, int height, double *buffer) {
	int i, j, k = 0;
	for (i = x; i < x + width; i += 1)
		for (j = y; j < y + height; j += 1) {
			buffer[k] = pixPixels[(i + pixWidth * j) * 3];
			buffer[k + 1] = pixPixels[(i + pixWidth * j) * 3 + 1];
			buffer[k + 2] = pixPixels[(i + pixWidth * j) * 3 + 2];
			k += 3;
		}
}*/



/*** Public: callbacks ***/

/* Sets a callback function for keys' being pressed. Invoked using something 
like
	pixSetKeyDownHandler(myKeyDownHandler);
where myKeyDownHandler is defined something like 
	void myKeyDownHandler(int key, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown);
The key parameter is GLFW_KEY_A, GLFW_KEY_B, ..., or GLFW_KEY_UNKNOWN, which 
are defined in GLFW/glfw3.h. The other parameters are flags describing the 
state of the modifier keys when the key was pressed. */
void pixSetKeyDownHandler(void (*handler)(int, int, int, int, int)) {
	pixUserKeyDownHandler = handler;
}

/* Sets a callback function for keys' being released. For details, see 
pixSetKeyDownHandler. */
void pixSetKeyUpHandler(void (*handler)(int, int, int, int, int)) {
	pixUserKeyUpHandler = handler;
}

/* Sets a callback function for keys' being held down. For details, see 
pixSetKeyDownHandler. */
void pixSetKeyRepeatHandler(void (*handler)(int, int, int, int, int)) {
	pixUserKeyRepeatHandler = handler;
}

/* Sets a callback function for mouse buttons' being pressed. Invoked using 
something like
	pixSetMouseDownHandler(myMouseDownHandler);
where myMouseDownHandler is defined something like 
	void myMouseDownHandler(int button, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown);
The button parameter is GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, or 
something like that. Those values are defined in GLFW/glfw3.h. The other 
parameters are flags describing the state of the modifier keys when the key was 
released. */
void pixSetMouseDownHandler(void (*handler)(int, int, int, int, int)) {
	pixUserMouseDownHandler = handler;
}

/* Sets a callback function for mouse buttons' being released. For details, see 
pixSetMouseDownHandler. */
void pixSetMouseUpHandler(void (*handler)(int, int, int, int, int)) {
	pixUserMouseUpHandler = handler;
}

/* Sets a callback function for the mouse's being moved. Invoked using 
something like
	pixSetMouseMoveHandler(myMouseMoveHandler);
where myMouseMoveHandler is defined something like 
	void myMouseMoveHandler(double x, double y);
The x and y parameters are relative to the lower left corner of the window. */
void pixSetMouseMoveHandler(void (*handler)(double, double)) {
	pixUserMouseMoveHandler = handler;
}

/* Sets a callback function for the mouse's being scrolled. Invoked using 
something like 
	pixSetMouseScrollHandler(myMouseScrollHandler);
where myMouseScrollHandler is defined something like 
	void myMouseScrollHandler(double xOffset, double yOffset);
A vertical scroll wheel will always report 0.0 for xOffset. A scrolling gesture 
on a trackpad might generate non-zero values for both xOffset and yOffset. */
void pixSetMouseScrollHandler(void (*handler)(double, double)) {
	pixUserMouseScrollHandler = handler;
}

/* Sets a callback function that fires once per animation frame, after all of 
the user interface callbacks. Invoked using something like 
	pixSetTimeStepHandler(myTimeStepHandler);
where myTimeStepHandler is defined something like
	void myTimeStepHandler(double oldTime, double newTime);
oldTime was the time for the previous frame; newTime is the time for the 
current frame. Both times are in seconds since the epoch (something like 1970). 
*/
void pixSetTimeStepHandler(void (*handler)(double, double)) {
	pixUserTimeStepHandler = handler;
}


