/* 010mainPainting.c 
   assignment for day 1 of cs311
   Ju Yun Kim*/


/* In this tutorial our compiler invocation becomes more complicated. On macOS,
we compile with...
    clang 000linking.c 000pixel.o -lglfw -framework OpenGL
This invocation causes our compiled code to be 'linked' with three other pieces
of software, that have been compiled elsewhere: OpenGL, GLFW, and pixel.o.
OpenGL is a cross-platform, standardized library for interacting with graphics
hardware. It comes pre-installed on macOS and Linux, and is easily installed on
Windows. GLFW is a cross-platform, minimalist user interface toolkit, that we
can use to build simple OpenGL applications. Often it is not pre-installed, but
it is easy to install. 000pixel.o is a library that I have written, to provide
a simple foundation for this graphics course. You must have 000pixel.o and
000pixel.h in the same directory as 000linking.c, for the linking to work. */

/* Notice that we include 000pixel.h. The file name is enclosed in quotation
marks rather than angle brackets, to tell the C compiler to search for the file
in the working directory. Feel free to read 000pixel.h. It documents all of the
public functionality of the library 000pixel.o. */
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "000pixel.h"

// G L O B A L S
bool mouseDown = false;
double r = 255.0;
double g = 255.0;
double b = 255.0;
double brushSize = 1;

void setColor(double rr, double gg, double bb) {
	r = rr;
	g = gg;
	b = bb;
}

void changeBrushSize(double offset) { 
	// size is is 1x1 3x3 5x5 ... with the cursor being the center pixel
	if (offset > 0) {
		if (brushSize < 99) {
			brushSize = brushSize + 2;	
		} 
	} else {
		if (brushSize > 1) {
			brushSize = brushSize - 2;	
		}
		
	}
}

/* handles the mouse drag draw, using the set color and brush size*/
void draw(double x, double y) {
	int i, j;
	double curX = x - ((brushSize - 1) / 2);
	double curY = y - ((brushSize - 1) / 2);
	for (i = 0; i < brushSize; i++) {
		for (j = 0; j < brushSize; j++) {
			pixSetRGB(curX, curY, r, g, b);
			curX = curX + 1.0;
		}
		curY = curY + 1.0;
	}
}

/* This function is a user interface 'callback'. Once we register it with the
user interface infrastructure, it is called whenever the user releases a
keyboard key. For details, see the pixSetKeyUpHandler documentation. */
void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown) {

	printf("key up %d, shift %d, control %d, altOpt %d, supComm %d\n",
		key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
	switch (key) {
		case (82): { //red
			setColor(255.0, 0.0, 0.0);
			break;
		} case (71): { //green
			setColor(0.0, 255.0, 0.0);
			break;
		} case (66): { //blue
			setColor(0.0, 0.0, 255.0);
			break;
		} case (67): { //cyan
			setColor(0.0, 255.0, 255.0);
			break;
		} case (77): { //magenta
			setColor(255.0, 0.0, 255.0);
			break;
		} case (89): {
			setColor(255.0, 255.0, 0.0);
			break;
		} case (75): { //key
			setColor(0.0, 0.0, 0.0);
			break;
		} case (87): { //white
			setColor(255.0, 255.0, 255.0);
			break;
		}
	}
}

/* Similarly, the following callbacks handle some mouse interactions. */
void handleMouseUp(int button, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown) {
	printf("mouse up %d, shift %d, control %d, altOpt %d, supComm %d\n",
		button, shiftIsDown, controlIsDown, altOptionIsDown,
		superCommandIsDown);
	mouseDown = false;
}

void handleMouseDown(int button, int shiftIsDown, int controlIsDown, int altOptionIsDown, int superCommandIsDown) {
	printf("mouse down %d, shift %d, control %d, altOpt %d, supComm %d\n",
	button, shiftIsDown, controlIsDown, altOptionIsDown,
	superCommandIsDown);
	mouseDown = true;
}

void handleMouseMove(double x, double y) {
	if (mouseDown) {
		// pixSetRGB(x, y, r, g, b);
		draw(x, y);
	}
	printf("mouse move x %f y %f\n", x, y);
}

void handleMouseScroll(double xOffset, double yOffset) {
	printf("mouse scroll xOffset %f yOffset %f\n", xOffset, yOffset);
}

/* This callback is called once per animation frame. As parameters it receives
the time for the current frame and the time for the previous frame. Both times
are measured in seconds since some distant past time. */
void handleTimeStep(double oldTime, double newTime) {
	if (floor(newTime) - floor(oldTime) >= 1.0)
		printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
}

/* You can also set callbacks for key down, key repeat, and mouse down. See
000pixel.h for details. */

int main(void) {
	/* Make a 512 x 512 window with the title 'Pixel Graphics'. This function
	returns 0 if no error occurred. */
	if (pixInitialize(512, 512, "Pixel Graphics") != 0)
		return 1;
	else {
		/* Register the callbacks (defined above) with the user interface, so
		that they are called as needed during pixRun (invoked below). */
		pixSetKeyUpHandler(handleKeyUp);
		pixSetMouseUpHandler(handleMouseUp);
		pixSetMouseDownHandler(handleMouseDown);
		pixSetMouseMoveHandler(handleMouseMove);
		pixSetMouseScrollHandler(handleMouseScroll);
		pixSetTimeStepHandler(handleTimeStep);
		/* Clear the window to black. */
		pixClearRGB(0.0, 0.0, 0.0);
		/* Draw the graph of y = 500 sin (x / 50) in yellow. Notice that
		pixSetRGB ignores attempts to set pixels outside the window. */
		int i;
		for (i = 0; i < 512; i = i + 1)
			pixSetRGB(i, 500.0 * sin(i / 50.0), 1.0, 1.0, 0.0);
		/* Run the event loop. The callbacks that were registered above are
		invoked as needed. At the end, the resources supporting the window are
		deallocated. */
		pixRun();
		return 0;
	}
}
