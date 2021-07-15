#include <iostream>
#include <cmath>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Scene.h"
#include "PLYReader.h"
#include "GL/glew.h"
#include  "Mesh.h"
Scene::Scene()
{
	mesh = NULL;
}

Scene::~Scene()
{
	if(mesh != NULL)
		delete mesh;
}


void Scene::init()
{
	initShaders();
	mesh = new TriangleMesh();
	mesh->buildCube();
	mesh->sendToOpenGL(basicProgram);
	currentTime = 0.0f;
	
	camera.init(1.0f);
	
	bPolygonFill = true;
	 n = 0;
	for (float i = 0; i <10; i += 2) {
		for (float j = 0; j < 10; j += 2) {
			
			locs.push_back( glm::translate(glm::mat4(1.0f), glm::vec3(i*2, 0, j*5)));
			n++;
		}
	}
	/*locs[0] = glm::translate(glm::mat4(1.0f), glm::vec3(0 * 2, 0, 0 * 2));
	locs[1] = glm::translate(glm::mat4(1.0f), glm::vec3(2 * 2, 0, 0 * 2));
	locs[2] = glm::translate(glm::mat4(1.0f), glm::vec3(0 * 2, 0, 2 * 2));
	locs[3] = glm::translate(glm::mat4(1.0f), glm::vec3(2 * 2, 0, 2 * 2));*/
 	for (int i = 0; i < n; ++i)
	{
		
     objects.push_back (new  Mesh("models/humen.obj", basicProgram, locs[i]));
	}
	
	glGenQueries(n, queries);
	// tile = new Mesh("models/humen.obj", basicProgram,locs[5]);
	 //wall = new Mesh("models/humen.obj", basicProgram, f2f);

	 //wall = new Mesh("models/wall.obj", basicProgram);
	// Select which font you want to use
	if(!text.init("fonts/OpenSans-Regular.ttf"))
	//if(!text.init("fonts/OpenSans-Bold.ttf"))
	//if(!text.init("fonts/DroidSerif.ttf"))
		cout << "Could not load font!!!" << endl;
}

bool Scene::loadMesh(const char *filename)
{
	PLYReader reader;
	
	mesh->free();
	bool bSuccess = reader.readMesh(filename, *mesh);
	if(bSuccess)
	  mesh->sendToOpenGL(basicProgram);
	
	return bSuccess;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
}
void Scene::BuildScene() {
	  
	
	glm::mat4 trans1;
	//trans1 = glm::scale(trans1, glm::vec3(0.8, 0.8, 0.8));

	glm::mat3 normalMatrix;

	basicProgram.use();
	basicProgram.setUniformMatrix4f("projection", glm::translate(camera.getProjectionMatrix(), glm::vec3(0)));
	basicProgram.setUniformMatrix4f("modelview", glm::translate(camera.getModelViewMatrix(), glm::vec3(0)));
	normalMatrix = glm::inverseTranspose(camera.getModelViewMatrix());
	basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
	
	 
}

void Scene::render()
{
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.5f, 0.0f, 0.0f));
	glm::mat3 normalMatrix;
	basicProgram.use();
	basicProgram.setUniformMatrix4f("projection", glm::translate(camera.getProjectionMatrix(), glm::vec3(0)));
	basicProgram.setUniformMatrix4f("modelview", glm::translate(camera.getModelViewMatrix(), glm::vec3(0)));
	normalMatrix = glm::inverseTranspose(camera.getModelViewMatrix());
	basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);

	basicProgram.setUniform1i("bLighting", bPolygonFill ? 1 : 0);
	
	if (bPolygonFill)
	{
		basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else
	{
		basicProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.5f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		mesh->render();
		glTranslatef(4.0f, 0, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_POLYGON_OFFSET_FILL);
		basicProgram.setUniform4f("color", 0.0f, 0.0f, 0.0f, 1.0f);

	}
	 
	if (OQ ) {
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		//glDepthMask(GL_FALSE);
		for (int i = 0; i < n; i++) {
			glBeginQuery(GL_SAMPLES_PASSED, queries[i]);
			objects[i]->render();
		    glEndQuery(GL_SAMPLES_PASSED);
		}
		glClear(GL_DEPTH_BUFFER_BIT);

		glFlush();
		//int i = 1 * 3 / 4; // instead of N-1, to prevent the GPU from going idle
		//do {
		//	glGetQueryObjectivARB(queries[i],
		//		GL_QUERY_RESULT_AVAILABLE_ARB,
		//		&available);
		//} while (!available);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);


		for (int i = 0; i < n; i++) {
			glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT,
				&sampleCount);
			if (sampleCount > 1000) {
				basicProgram.setUniform4f("color", 0.9*sampleCount / 307200, 0.1f, 0.1f, 1.0f);

				objects[i]->render();

			}

		}


	}
	else {
		for (int i = 0; i < n; i++) {

			glBeginQuery(GL_SAMPLES_PASSED, queries[i]);
			objects[i]->render();


			glEndQuery(GL_SAMPLES_PASSED);
		}


	}

	 	text.render("Mesh renderer", glm::vec2(20, 20), 16, glm::vec4(0, 0, 0, 1));
}

Camera &Scene::getCamera()
{
  return camera;
}

void Scene::switchPolygonMode()
{
  bPolygonFill = !bPolygonFill;
}


void Scene::switchOQMode() {
	OQ = !OQ;
}
void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/basic.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/basic.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	basicProgram.init();
	basicProgram.addShader(vShader);
	basicProgram.addShader(fShader);
	basicProgram.link();
	if(!basicProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << basicProgram.log() << endl << endl;
	}
	basicProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}



