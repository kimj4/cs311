typedef struct renRenderer renRenderer;

struct renRenderer {
	int unifDim; // dimension of stuff properties that apply to the whole triangle
	int texNum;  // number of textures
	int attrDim; // dimension of information specific to each vertex
  int varyDim;
	// function pointers
  void (*colorPixel)(renRenderer *, double[], texTexture *[], double[], double[]);
  void (*transformVertex)(renRenderer *, double[], double[], double[]);
	void (*updateUniform)(renRenderer *, double[], double[]);
};
