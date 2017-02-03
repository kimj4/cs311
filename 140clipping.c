void clipRender(renRenderer *ren, double unif[], texTexture *tex[],
	double a[], double b[], double c[]) {

		// if (a[renVARYZ] > a[renVARYW] || a[renVARYW] <= 0)
		if (a[renVARYZ] <= a[renVARYW] || a[renVARYW] > 0) {
			vecScale(4, 1 / a[renVARYW], a, a);

			vecScale(4, 1 / b[renVARYW], b, b);

			vecScale(4, 1 / c[renVARYW], c, c);

			double tempA[4], tempB[4], tempC[4];
			mat441Multiply(ren->viewport, a, tempA);
			mat441Multiply(ren->viewport, b, tempB);
			mat441Multiply(ren->viewport, c, tempC);
			//
			// a[renVARYX] = tempA[0];
			// b[renVARYX] = tempB[0];
			// c[renVARYX] = tempC[0];
			//
			// a[renVARYY] = tempA[1];
			// b[renVARYY] = tempB[1];
			// c[renVARYY] = tempC[1];
			//
			// a[renVARYZ] = tempA[2];
			// b[renVARYZ] = tempB[2];
			// c[renVARYZ] = tempC[2];
			//
			// a[renVARYZ] = tempA[3];
			// b[renVARYZ] = tempB[3];
			// c[renVARYZ] = tempC[3];

			// vecPrint(renVARYDIM, a);
			// mat44Print(ren->viewport);

			// vecCopy(4, tempA, a);
			// vecCopy(4, tempB, b);
			// vecCopy(4, tempC, c);

			// triRender(ren, unif, tex, tempA, tempB, tempC);
			triRender(ren, unif, tex, a, b, c);
		}
}
