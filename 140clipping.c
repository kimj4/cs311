/*
 * 140clipping.c
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * handles clipping at the near plane.
 */


/* helper function that does the homogenous division and viewport transformation
   for the input vertex*/
void processVerts(renRenderer *ren, double inputVert[], double outputVert[]) {
	double scaled[renVARYDIM], temp[renVARYDIM];
	vecScale(renVARYDIM, 1 / inputVert[renVARYW], inputVert, scaled);
	mat441Multiply(ren->viewport, scaled, temp);
	vecCopy(renVARYDIM, inputVert, outputVert);
	vecCopy(4, temp, outputVert);
}

/* helper function that checks the clipping condition for the vertex */
int isClipped(double vary[]) {
	if (vary[renVARYZ] > vary[renVARYW] || vary[renVARYW] <= 0) {
		return 1;
	} else {
		return 0;
	}
}

/* helper function that does the interpolation for the intersection of the
   triangle and the clipping plane. v1 is clipped, v2 is not.
	 The interpolation is done on the whole vector*/
void findIntersect(double v1[], double v2[], double result[]) {
	double t =  (v1[3] - v1[2]) / (v1[3] - v1[2] + v2[2] - v2[3]);
	double v2MinusV1[renVARYDIM];
	vecSubtract(renVARYDIM, v2, v1, v2MinusV1);
	vecScale(renVARYDIM, t, v2MinusV1, result);
	vecAdd(renVARYDIM, v1, result, result);
}

/* function that handles clipping for a triangle. After the course of action handles
   been determined, triRender is called */
void clipRender(renRenderer *ren, double unif[], texTexture *tex[],
	double a[], double b[], double c[]) {
			int aClipped, bClipped, cClipped;
			aClipped = isClipped(a);
			bClipped = isClipped(b);
			cClipped = isClipped(c);

			// handle the 8 distinct cases of vertices being clipped
			if (aClipped && bClipped && cClipped) {
				// don't render anything
			} else if (aClipped && bClipped && !cClipped) {
				double dAC[renVARYDIM], dBC[renVARYDIM];
				double newDAC[renVARYDIM], newDBC[renVARYDIM], newC[renVARYDIM];
				findIntersect(a, c, dAC);
				findIntersect(b, c, dBC);
				processVerts(ren, dAC, newDAC);
				processVerts(ren, dBC, newDBC);
				processVerts(ren, c, newC);
				triRender(ren, unif, tex, newDAC, newDBC, newC);

			} else if (aClipped && (!bClipped) && cClipped) {
				double dAB[renVARYDIM], dCB[renVARYDIM];
				double newDAB[renVARYDIM], newB[renVARYDIM], newDCB[renVARYDIM];
				findIntersect(a, b, dAB);
				findIntersect(c, b, dCB);
				processVerts(ren, dAB, newDAB);
				processVerts(ren, b, newB);
				processVerts(ren, dCB, newDCB);
				triRender(ren, unif, tex, newDAB, newB, newDCB);

			} else if ((!aClipped) && bClipped && cClipped) {
				double dBA[renVARYDIM], dCA[renVARYDIM];
				double newA[renVARYDIM], newDBA[renVARYDIM], newDCA[renVARYDIM];
				findIntersect(b, a, dBA);
				findIntersect(c, a, dCA);
				processVerts(ren, a, newA);
				processVerts(ren, dBA, newDBA);
				processVerts(ren, dCA, newDCA);
				triRender(ren, unif, tex, newA, newDBA, newDCA);

			} else if (aClipped && (!bClipped) && (!cClipped)) {
				double dAB[renVARYDIM], dAC[renVARYDIM];
				double newDAB[renVARYDIM], newDAC[renVARYDIM], newB[renVARYDIM], newC[renVARYDIM];
				findIntersect(a, b, dAB);
				findIntersect(a, c, dAC);
				processVerts(ren, dAB, newDAB);
				processVerts(ren, dAC, newDAC);
				processVerts(ren, b, newB);
				processVerts(ren, c, newC);
				triRender(ren, unif, tex, newDAB, newB, newC);
				triRender(ren, unif, tex, newDAC, newB, newC);

			} else if ((!aClipped) && (!bClipped) && cClipped) {
				double dCA[renVARYDIM], dCB[renVARYDIM];
				double newDCA[renVARYDIM], newDCB[renVARYDIM], newA[renVARYDIM], newB[renVARYDIM];
				findIntersect(c, a, dCA);
				findIntersect(c, b, dCB);
				processVerts(ren, a, newA);
				processVerts(ren, b, newB);
				processVerts(ren, dCA, newDCA);
				processVerts(ren, dCB, newDCB);
				triRender(ren, unif, tex, newA, newB, newDCA);
				triRender(ren, unif, tex, newA, newB, newDCB);


			} else if ((!aClipped) && bClipped && (!cClipped)) {
				double dBA[renVARYDIM], dBC[renVARYDIM];
				double newDBA[renVARYDIM], newDBC[renVARYDIM], newA[renVARYDIM], newC[renVARYDIM];
				findIntersect(b, a, dBA);
				findIntersect(b, c, dBC);
				processVerts(ren, a, newA);
				processVerts(ren, dBA, newDBA);
				processVerts(ren, dBC, newDBC);
				processVerts(ren, c, newC);
				triRender(ren, unif, tex, newA, newDBA, newC);
				triRender(ren, unif, tex, newA, newDBC, newC);

			} else if ((!aClipped) && (!bClipped) && (!cClipped)) {
				double newA[renVARYDIM], newB[renVARYDIM], newC[renVARYDIM];
				processVerts(ren, a, newA);
				processVerts(ren, b, newB);
				processVerts(ren, c, newC);
				vecPrint(renVARYDIM, newA);
				triRender(ren, unif, tex, newA, newB, newC);
			} else {
				printf("ERROR: clipRender doesn't know what vertices are clipped\n");
			}
}
