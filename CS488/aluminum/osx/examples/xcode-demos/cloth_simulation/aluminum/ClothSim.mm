#include "Aluminum/Includes.hpp"

#include "Aluminum/Program.hpp"
#include "Aluminum/MeshBuffer.hpp"
#include "Aluminum/MeshData.hpp"
#include "Aluminum/Shapes.hpp"
#include "Aluminum/Camera.hpp"
#include "Aluminum/Utils.hpp"
#include "Aluminum/MeshUtils.hpp"
#include "Aluminum/FBO.hpp"
#include "Aluminum/Behavior.hpp"
#include "Aluminum/ResourceHandler.h"


using namespace aluminum;

class RubiksCube : public RendererOSX {

public:

	ResourceHandler rh;
	typedef struct cubeStruct{

		// MeshBuffer cubeMeshBuffer;
//		mat4 cubeModel;
		float rotateX;
		float rotateY;
		float rotateZ;
		float xpos;
		float ypos;
		float zpos;

	} cubeStruct;

	typedef cubeStruct* cubesPtr;

	Program programColor;
	GLint posLoc = 0, normalLoc = 1, texCoordLoc = 2, colorLoc = 3;
	mat4 model, view, proj;
	// float rotateX[27], rotateY[27], rotateZ[27];
	MeshData md;
	 MeshBuffer cubeMeshBuffer;
	cubesPtr* cubes;


	vec3 l1_diffuse = vec3(0.0,1.0,0.0);
	vec3 l1_specular = vec3(1.0,1.0,1.0);

	vec3 l2_diffuse = vec3(0.0,0.0,1.0);
	vec3 l2_specular = vec3(1.0,1.0,1.0);


	void onCreate() {

		rh.loadProgram(programColor, "color", posLoc, normalLoc, -1, colorLoc);

		// MeshData md3;
		cubes = (cubesPtr*)malloc(27*sizeof(cubesPtr));
		for(int i = 0; i < 27; ++i)
			cubes[i] = (cubesPtr)malloc(sizeof(cubeStruct));

		addCube(md, 0.1);
		cubeMeshBuffer.init(md, posLoc, normalLoc, -1, colorLoc);

		float xpos = 0.25f, ypos = 0.25f, zpos = 0.25f;
		for(int i = 0; i < 27; ++i) {
			// addCube(md[i], 0.1); //this version makes normals, texcoords, and colors each side with a different default color
			// cubes[i]->cubeMeshBuffer.init(md[i], posLoc, normalLoc, -1, colorLoc);
			cubes[i]->xpos = xpos;
			cubes[i]->ypos = ypos;
			cubes[i]->zpos = zpos;
			cubes[i]->rotateX = 0.0f;
			cubes[i]->rotateY = 0.0f;
			cubes[i]->rotateZ = 0.0f;

			if((i+1)%3 == 0 && i != 0) {
				xpos = 0.25f;
				if((i+1)%9 == 0) {
					ypos = 0.25f;
					zpos -= 0.25f;
				}
				else
					ypos -= 0.25f;
			}
			else
				xpos -= 0.25f;

		}

		proj = glm::perspective(glm::radians(60.0f), float(width)/float(height), 0.1f, 100.0f);
		view = glm::lookAt(vec3(0.0,0.0,3), vec3(0,0,0), vec3(0,1,0) );
		model = glm::mat4();

		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, width, height);

	}

	void draw(){

		handleMouse();

		model = glm::mat4(1.0);

		glViewport(0, 0, width, height);
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		 float xpos = 0.25f, ypos = 0.25f, zpos = 0.25f;
		// draw 27 colored cube
		 // cout << "cube rotations: " << endl;
		for(int i = 0; i < 27; ++i) {

			model = mat4(1.0);
			model = glm::translate(model, vec3(xpos,ypos,zpos));
			// rotate cubes around z, y, or x axises in order to get the right colors in their spots
			model = glm::rotate(model, cubes[i]->rotateX, vec3(0.0, 0.0, 1.0));
			model = glm::rotate(model, cubes[i]->rotateY, vec3(0.0, 1.0, 0.0));
			model = glm::rotate(model, cubes[i]->rotateZ, vec3(1.0, 0.0, 0.0));

			// cout << rotateX[i] << " " << rotateY[i] << " " << rotateZ[i] << endl;

			programColor.bind(); {

				glUniformMatrix4fv(programColor.uniform("model"), 1, 0, ptr(model));
				glUniformMatrix4fv(programColor.uniform("view"), 1, 0, ptr(view));
				glUniformMatrix4fv(programColor.uniform("proj"), 1, 0, ptr(proj));

				cubeMeshBuffer.draw();

			} programColor.unbind();

			if((i+1)%3 == 0 && i != 0) {
				xpos = 0.25f;
				if((i+1)%9 == 0) {
					ypos = 0.25f;
					zpos -= 0.25f;
				}
				else
					ypos -= 0.25f;
			}
			else
				xpos -= 0.25f;

		}

	}

  	void onFrame() {

		glViewport(0, 0, width, height); {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			draw();
		}

  	}

  	float dirX = 0.0, dirY = 0.0, dirZ = 0.0;
	void handleMouse() {

		if(isDragging) {

			if(previousMouseX-mouseX > 0)
				dirX = 0.001;
			else if(previousMouseX-mouseX < 0)
				dirX = -0.001;

			if(previousMouseY-mouseY > 0)
				dirY = 0.001;
			else if(previousMouseY-mouseY < 0)
				dirY = -0.001;

			view = glm::rotate(view, float(dirX*mouseX), vec3(0.0, 1.0, 0.0));
			view = glm::rotate(view, float(dirY*mouseY), vec3(1.0, 0.0, 0.0));

		}
		isDragging = false;

	}

	virtual void keyDown(char key) {

		cubesPtr * temp;
		temp = (cubesPtr*)malloc(9*sizeof(cubesPtr));
		for(int i = 0; i < 9; ++i)
			temp[i] = (cubesPtr)malloc(sizeof(cubeStruct));
		switch(key) {
			case kVK_Space:
				view = glm::lookAt(vec3(0.0,0.0,3), vec3(0,0,0), vec3(0,1,0) );
				break;
			case kVK_ANSI_F:
				//rotate front face to the left
				temp[0] = cubes[6];
				temp[1] = cubes[3];
				temp[2] = cubes[0];
				temp[3] = cubes[7];
				temp[4] = cubes[4];
				temp[5] = cubes[1];
				temp[6] = cubes[8];
				temp[7] = cubes[5];
				temp[8] = cubes[2];
				for(int i = 0; i < 9; ++i) {
					cubes[i] = temp[i];
					cubes[i]->rotateX += M_PI/2;
				}
 				break;
			case kVK_ANSI_T:
				//rotate top to the right
				temp[0] = cubes[0];
			 	temp[1] = cubes[9];
			 	temp[2] = cubes[18];
			 	temp[3] = cubes[1];
			 	temp[4] = cubes[10];
			 	temp[5] = cubes[19];
			 	temp[6] = cubes[2];
			 	temp[7] = cubes[11];
			 	temp[8] = cubes[20];

			 	cubes[0] = temp[6];
			 	cubes[1] = temp[7];
			 	cubes[2] = temp[8];
			 	cubes[9] = temp[3];
			 	cubes[10] = temp[4];
			 	cubes[11] = temp[5];
			 	cubes[18] = temp[0];
			 	cubes[19] = temp[1];
			 	cubes[20] = temp[2];
			 	for(int i = 0; i < 21; ++i) {

					cubes[i]->rotateY += M_PI/2;
					if(i == 2 || i == 11)
						i += 6;

				}
				break;
			case kVK_ANSI_B:
			 	//rotate bottom to the right
				temp[0] = cubes[8];
				temp[1] = cubes[17];
				temp[2] = cubes[26];
				temp[3] = cubes[7];
				temp[5] = cubes[25];
				temp[6] = cubes[6];
				temp[7] = cubes[15];
				temp[8] = cubes[24];

				cubes[6] = temp[0];
				cubes[7] = temp[1];
				cubes[8] = temp[2];
				cubes[15] = temp[3];
				cubes[17] = temp[5];
				cubes[24] = temp[6];
				cubes[25] = temp[7];
				cubes[26] = temp[8];
				for(int i = 6; i < 27; ++i) {

					cubes[i]->rotateY += M_PI/2;
					if(i == 8 || i == 17)
						i += 6;

				}
				break;
		}
	}

};

int main(){
  return RubiksCube().start("aluminum::Rubiks Cube", 100, 100, 800, 600);
}
