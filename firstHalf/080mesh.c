double varyVerts[renVARYDIMBOUND * renVERTNUMBOUND];

void transformVertex(renRenderer *ren, double unif[], double attr[],
        double vary[]);

/*** Creating and destroying ***/

/* Feel free to read the struct's members, but don't write them, except through
the accessors below such as meshSetTriangle, meshSetVertex. */
typedef struct meshMesh meshMesh;
struct meshMesh {
  int triNum, vertNum, attrDim;
  int *tri;     /* triNum * 3 ints */
  double *vert; /* vertNum * attrDim doubles */
};

/* Initializes a mesh with enough memory to hold its triangles and vertices.
Does not actually fill in those triangles or vertices with useful data. When
you are finished with the mesh, you must call meshDestroy to deallocate its
backing resources. */
int meshInitialize(meshMesh *mesh, int triNum, int vertNum, int attrDim) {
  mesh->tri = (int *)malloc(triNum * 3 * sizeof(int) +
                            vertNum * attrDim * sizeof(double));
  if (mesh->tri != NULL) {
    mesh->vert = (double *)&(mesh->tri[triNum * 3]);
    mesh->triNum = triNum;
    mesh->vertNum = vertNum;
    mesh->attrDim = attrDim;
  }
  return (mesh->tri == NULL);
}

/* Sets the trith triangle to have vertex indices i, j, k. */
void meshSetTriangle(meshMesh *mesh, int tri, int i, int j, int k) {
  if (0 <= tri && tri < mesh->triNum) {
    mesh->tri[3 * tri] = i;
    mesh->tri[3 * tri + 1] = j;
    mesh->tri[3 * tri + 2] = k;
  }
}

/* Returns a pointer to the trith triangle. For example:
        int *triangle13 = meshGetTrianglePointer(&mesh, 13);
        printf("%d, %d, %d\n", triangle13[0], triangle13[1], triangle13[2]); */
int *meshGetTrianglePointer(meshMesh *mesh, int tri) {
  if (0 <= tri && tri < mesh->triNum)
    return &mesh->tri[tri * 3];
  else
    return NULL;
}

/* Sets the vertth vertex to have attributes attr. */
void meshSetVertex(meshMesh *mesh, int vert, double attr[]) {
  int k;
  if (0 <= vert && vert < mesh->vertNum)
    for (k = 0; k < mesh->attrDim; k += 1)
      mesh->vert[mesh->attrDim * vert + k] = attr[k];
}

/* Returns a pointer to the vertth vertex. For example:
        double *vertex13 = meshGetVertexPointer(&mesh, 13);
        printf("x = %f, y = %f\n", vertex13[0], vertex13[1]); */
double *meshGetVertexPointer(meshMesh *mesh, int vert) {
  if (0 <= vert && vert < mesh->vertNum)
    return &mesh->vert[vert * mesh->attrDim];
  else
    return NULL;
}

double *meshGetVaryVertexPointer(meshMesh *mesh, int vert) {
  if (0 <= vert && vert < mesh->vertNum)
    return &varyVerts[vert * mesh->attrDim];
  else
    return NULL;
}

/* Deallocates the resources backing the mesh. This function must be called
when you are finished using a mesh. */
void meshDestroy(meshMesh *mesh) { free(mesh->tri); }

/*** Rendering ***/

/* Renders the mesh. If the mesh and the renderer have differing values for
attrDim, then prints an error message and does not render anything. */
void meshRender(meshMesh *mesh, renRenderer *ren, double unif[],
                texTexture *tex[]) {
  if (mesh->attrDim != ren->attrDim) {
    printf("meshRender error: mesh attrDim and ren attrDim are unequal\n");
    printf("mesh attrDim: %i, ren attrDim: %i\n", mesh->attrDim, ren->attrDim);
    return;
  } else {
    int j;
    for (j = 0; j < mesh->vertNum; j++) { // iterate through all vertices
      double tempVaryVert[renVARYDIMBOUND];
      transformVertex(ren, unif, meshGetVertexPointer(mesh, j), tempVaryVert);
      int k;
      for (k = 0; k < mesh->attrDim; k++) {
        varyVerts[(mesh->attrDim * j)  + k] = tempVaryVert[k];
      }
    }
    int i, *curTri;
    for (i = 0; i < mesh->triNum; i++) {
      curTri = meshGetTrianglePointer(mesh, i);
      // printf("curTri: (%d, %d, %d)\n", curTri[0], curTri[1], curTri[2]);
      triRender(ren, unif, tex, meshGetVaryVertexPointer(mesh, curTri[0]),
                                meshGetVaryVertexPointer(mesh, curTri[1]),
                                meshGetVaryVertexPointer(mesh, curTri[2]));
    }
  }
}

/*** Convenience initializers: 2D ***/

/* Initializes a mesh to two triangles forming a rectangle of the given sides.
The four attributes are X, Y, S, T. Do not call meshInitialize separately; it
is called inside this function. Don't forget to call meshDestroy when done. */
int meshInitializeRectangle(meshMesh *mesh, double left, double right,
                            double bottom, double top) {
  int error = meshInitialize(mesh, 2, 4, 2 + 2);
  if (error == 0) {
    meshSetTriangle(mesh, 0, 0, 1, 2);
    meshSetTriangle(mesh, 1, 0, 2, 3);
    double attr[4];
    vecSet(4, attr, left, bottom, 0.0, 0.0);
    meshSetVertex(mesh, 0, attr);
    vecSet(4, attr, right, bottom, 1.0, 0.0);
    meshSetVertex(mesh, 1, attr);
    vecSet(4, attr, right, top, 1.0, 1.0);
    meshSetVertex(mesh, 2, attr);
    vecSet(4, attr, left, top, 0.0, 1.0);
    meshSetVertex(mesh, 3, attr);
  }
  return error;
}

/* Initializes a mesh to sideNum triangles forming an ellipse of the given
center (x, y) and radii rx, ry. The four attributes are X, Y, S, T. Do not call
meshInitialize separately; it is called inside this function. Don't forget to
call meshDestroy when done. */
int meshInitializeEllipse(meshMesh *mesh, double x, double y, double rx,
                          double ry, int sideNum) {
  int i, error;
  double theta, cosTheta, sinTheta, attr[4] = {x, y, 0.5, 0.5};
  error = meshInitialize(mesh, sideNum, sideNum + 1, 2 + 2);
  if (error == 0) {
    meshSetVertex(mesh, 0, attr);
    for (i = 0; i < sideNum; i += 1) {
      meshSetTriangle(mesh, i, 0, i + 1, (i + 1) % sideNum + 1);
      theta = i * 2.0 * M_PI / sideNum;
      cosTheta = cos(theta);
      sinTheta = sin(theta);
      vecSet(4, attr, x + rx * cosTheta, y + ry * sinTheta,
             0.5 * cosTheta + 0.5, 0.5 * sinTheta + 0.5);
      meshSetVertex(mesh, i + 1, attr);
    }
  }
  return error;
}
