/*** Creation and destruction ***/

/* Feel free to read from this struct's members, but don't write to them except
through the accessor functions. */
typedef struct sceneNode sceneNode;
struct sceneNode {
	GLdouble rotation[3][3];
	GLdouble translation[3];
	GLuint unifDim;
	GLdouble *unif;
	meshGLMesh *meshGL;
	sceneNode *firstChild, *nextSibling;
};

/* Initializes a sceneNode struct. The translation and rotation are initialized to trivial values. The user must remember to call sceneDestroy or
sceneDestroyRecursively when finished. Returns 0 if no error occurred. */
int sceneInitialize(sceneNode *node, GLuint unifDim, meshGLMesh *mesh,
		sceneNode *firstChild, sceneNode *nextSibling) {
	node->unif = (GLdouble *)malloc(unifDim * sizeof(GLdouble));
	if (node->unif == NULL)
		return 1;
	mat33Identity(node->rotation);
	vecSet(3, node->translation, 0.0, 0.0, 0.0);
	node->unifDim = unifDim;
	node->meshGL = mesh;
	node->firstChild = firstChild;
	node->nextSibling = nextSibling;
	return 0;
}

/* Deallocates the resources backing this scene node. Does not destroy the
resources backing the mesh, etc. */
void sceneDestroy(sceneNode *node) {
	if (node->unif != NULL)
		free(node->unif);
	node->unif = NULL;
}



/*** Accessors ***/

/* Copies the unifDim-dimensional vector from unif into the node. */
void sceneSetUniform(sceneNode *node, double unif[]) {
	vecCopy(node->unifDim, unif, node->unif);
}

/* Sets one uniform in the node, based on its index in the unif array. */
void sceneSetOneUniform(sceneNode *node, int index, double unif) {
	node->unif[index] = unif;
}

/* Calls sceneDestroy recursively on the node's descendants and younger
siblings, and then on the node itself. */
void sceneDestroyRecursively(sceneNode *node) {
	if (node->firstChild != NULL)
		sceneDestroyRecursively(node->firstChild);
	if (node->nextSibling != NULL)
		sceneDestroyRecursively(node->nextSibling);
	sceneDestroy(node);
}

/* Sets the node's rotation. */
void sceneSetRotation(sceneNode *node, GLdouble rot[3][3]) {
	vecCopy(9, (GLdouble *)rot, (GLdouble *)(node->rotation));
}

/* Sets the node's translation. */
void sceneSetTranslation(sceneNode *node, GLdouble transl[3]) {
	vecCopy(3, transl, node->translation);
}

/* Sets the scene's mesh. */
void sceneSetMesh(sceneNode *node, meshGLMesh *mesh) {
	node->meshGL = mesh;
}

/* Sets the node's first child. */
void sceneSetFirstChild(sceneNode *node, sceneNode *child) {
	node->firstChild = child;
}

/* Sets the node's next sibling. */
void sceneSetNextSibling(sceneNode *node, sceneNode *sibling) {
	node->nextSibling = sibling;
}

/* Adds a sibling to the given node. The sibling shows up as the youngest of
its siblings. */
void sceneAddSibling(sceneNode *node, sceneNode *sibling) {
	if (node->nextSibling == NULL)
		node->nextSibling = sibling;
	else
		sceneAddSibling(node->nextSibling, sibling);
}

/* Adds a child to the given node. The child shows up as the youngest of its
siblings. */
void sceneAddChild(sceneNode *node, sceneNode *child) {
	if (node->firstChild == NULL)
		node->firstChild = child;
	else
		sceneAddSibling(node->firstChild, child);
}

/* Removes a sibling from the given node. Equality of nodes is assessed as
equality of pointers. If the sibling is not present, then has no effect (fails
silently). */
void sceneRemoveSibling(sceneNode *node, sceneNode *sibling) {
	if (node->nextSibling == NULL)
		return;
	else if (node->nextSibling == sibling)
		node->nextSibling = sibling->nextSibling;
	else
		sceneRemoveSibling(node->nextSibling, sibling);
}

/* Removes a child from the given node. Equality of nodes is assessed as
equality of pointers. If the sibling is not present, then has no effect (fails
silently). */
void sceneRemoveChild(sceneNode *node, sceneNode *child) {
	if (node->firstChild == NULL)
		return;
	else if (node->firstChild == child)
		node->firstChild = child->nextSibling;
	else
		sceneRemoveSibling(node->firstChild, child);
}



/*** OpenGL ***/



/* Renders the node, its younger siblings, and their descendants. parent is the
modeling matrix at the parent of the node. If the node has no parent, then this
matrix is the 4x4 identity matrix. Loads the modeling transformation into
modelingLoc. The attribute information exists to be passed to meshGLRender. The
uniform information is analogous, but sceneRender loads it, not meshGLRender. */
void sceneRender(sceneNode *node, GLdouble parent[4][4], GLint modelingLoc,
		GLuint unifNum, GLuint unifDims[], GLint unifLocs[],
		GLuint attrNum, GLuint attrDims[], GLint attrLocs[]) {

	/* Set the uniform modeling matrix. */
	GLdouble selfIsom[4][4], parentMultiplied[4][4];
	GLfloat pmGL[4][4];
	mat44Isometry(node->rotation, node->translation, selfIsom);
	mat444Multiply(parent, selfIsom, parentMultiplied);
	mat44OpenGL(parentMultiplied, pmGL);
	glUniformMatrix4fv(modelingLoc, 1, GL_FALSE, (GLfloat *)pmGL);

	/* Set the other uniforms. The casting from double to float is annoying. */
	int i;
	int curUnifIdx = 0;
	for (i = 0; i < unifNum; i++) {
		switch(unifDims[i]) {
			case(1): {
				GLfloat unif[1];
				vecOpenGL(1, &node->unif[curUnifIdx], unif);
				glUniform1fv(unifLocs[i], 1, unif);
				curUnifIdx += 1;
				break;
			}
			case(2): {
				GLfloat unif[2];
				vecOpenGL(2, &node->unif[curUnifIdx], unif);
				glUniform2fv(unifLocs[i], 1, unif);
				curUnifIdx += 2;
				break;
			}
			case(3): {
				GLfloat unif[3];
				vecOpenGL(3, &node->unif[curUnifIdx], unif);
				glUniform3fv(unifLocs[i], 1, unif);
				curUnifIdx += 3;
				break;
			}
			case(4): {
				GLfloat unif[4];
				vecOpenGL(4, &node->unif[curUnifIdx], unif);
				glUniform4fv(unifLocs[i], 1, unif);
				curUnifIdx += 4;
				break;
			}
			default: {
				printf("Scene Error: A uniform dimension exceeds 4\n");
				break;
			}
		}
	}

	/* Render the mesh, the children, and the younger siblings. */
	meshGLRender(node->meshGL, attrNum, attrDims, attrLocs);

	if (node->firstChild != NULL) {
		sceneRender(node->firstChild, parentMultiplied, modelingLoc,
								unifNum, unifDims, unifLocs,
								attrNum, attrDims, attrLocs);
	}
	if (node->nextSibling != NULL) {
		sceneRender(node->nextSibling, parent, modelingLoc,
								unifNum, unifDims, unifLocs,
								attrNum, attrDims, attrLocs);
	}
}
