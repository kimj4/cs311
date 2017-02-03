int isClipped(double vary[]) {
	if (vary[renVARYZ] > vary[renVARYW] || vary[renVARYW] <= 0) {
		return 1;
	} else {
		return 0;
	}
}

/* helper function that does the interpolation for the intersection of the
   triangle and the clipping plane. v1 is clipped, v2 is not. */
void findIntersect(double v1[], double v2[], double result[]) {
	double t =  (v1[3] - v1[2]) / (v1[3] - v1[2] + v2[2] - v2[3]);
	double v2MinusV1[renVARYDIM];
	vecSubtract(renVARYDIM, v2, v1, v2MinusV1);
	vecScale(renVARYDIM, t, v2MinusV1, result);
	vecAdd(renVARYDIM, v1, result, result);
}

void clipRender(renRenderer *ren, double unif[], texTexture *tex[],
	double a[], double b[], double c[]) {
			int aClipped, bClipped, cClipped;
			aClipped = isClipped(a);
			bClipped = isClipped(b);
			cClipped = isClipped(c);
			// printf("%i\n", 1 && 0);
			// printf("%i\n", 1 && 1);
			// printf("%i\n", 1 && !0);

			if (aClipped && bClipped && cClipped) {
				printf("a b c\n");
				// don't render anything
			} else if ((aClipped) && (bClipped) && (!cClipped)) {
				printf("a b notc\n");
				double dAC[renVARYDIM], dBC[renVARYDIM];

				findIntersect(a, c, dAC);
				findIntersect(b, c, dAC);
				// emits one triangle

			} else if (aClipped && (!bClipped) && cClipped) {
				printf("a notb c\n");

			} else if ((!aClipped) && bClipped && cClipped) {
				printf("nota b c\n");

			} else if (aClipped && (!bClipped) && (!cClipped)) {
				printf("a notb notc\n");

			} else if ((!aClipped) && (!bClipped) && cClipped) {
				printf("nota notb c\n");

			} else if ((!aClipped) && bClipped && (!cClipped)) {
				printf("nota b notc\n");

			} else if ((!aClipped) && (!bClipped) && (!cClipped)) {
				// printf("nota notb notc\n");
				// printf("nothing is clipped\n");
				// render full triangle
				vecScale(4, 1 / a[renVARYW], a, a);

				vecScale(4, 1 / b[renVARYW], b, b);

				vecScale(4, 1 / c[renVARYW], c, c);

				double tempA[4], tempB[4], tempC[4];
				mat441Multiply(ren->viewport, a, tempA);
				mat441Multiply(ren->viewport, b, tempB);
				mat441Multiply(ren->viewport, c, tempC);
				a[renVARYX] = tempA[0];
				b[renVARYX] = tempB[0];
				c[renVARYX] = tempC[0];

				a[renVARYY] = tempA[1];
				b[renVARYY] = tempB[1];
				c[renVARYY] = tempC[1];

				a[renVARYZ] = tempA[2];
				b[renVARYZ] = tempB[2];
				c[renVARYZ] = tempC[2];

				a[renVARYZ] = tempA[3];
				b[renVARYZ] = tempB[3];
				c[renVARYZ] = tempC[3];

				// vecPrint(renVARYDIM, a);
				// mat44Print(ren->viewport);

				// vecCopy(4, tempA, a);
				// vecCopy(4, tempB, b);
				// vecCopy(4, tempC, c);

				// triRender(ren, unif, tex, tempA, tempB, tempC);
				triRender(ren, unif, tex, a, b, c);
			} else {
				printf("ERROR: clipRender doesn't know what vertices are clipped\n");
			}
}
