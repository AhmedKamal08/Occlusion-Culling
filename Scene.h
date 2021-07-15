#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "Camera.h"
#include "ShaderProgram.h"
#include "TriangleMesh.h"
#include "Text.h"
#include "Mesh.h"

// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	bool loadMesh(const char *filename);
	void update(int deltaTime);
	void render();
	void   BuildScene();

  Camera &getCamera();
public: int n; 
  void switchPolygonMode();
  void switchOQMode();

private:
	void initShaders();
	void computeModelViewMatrix();

private:
  Camera camera;
	TriangleMesh *mesh;
	ShaderProgram basicProgram;
	float currentTime;
	int obnum;
	bool bPolygonFill;
	bool OQ = true;
     // Mesh *objects[1000];
	  std::vector<Mesh*> objects;

      Mesh *tile ;
  //  Mesh *wall;
	  Text text;
	 // glm::mat4 locs[1000];
	  std::vector<glm::mat4> locs;

	  GLuint queries[1000];
	  GLuint sampleCount;
	  GLint available;
};


#endif // _SCENE_INCLUDE

