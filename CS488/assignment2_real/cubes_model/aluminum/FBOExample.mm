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

class FBOExample : public RendererOSX {

public:

	ResourceHandler rh;

	Camera camera;
	Program textureProgram, programColor;
	GLint posLoc = 0, normalLoc = 1, texCoordLoc = 2, colorLoc = 3;
	mat4 model, view, proj;
//	MeshData mesh1, mesh2;
//	MeshBuffer mb1, mb2;
	FBO fbo;
	vector<MeshBuffer> cubeMeshBuffer1, cubeMeshBuffer2;
	vector<Texture> textures;
	Behavior rotateBehavior;
	
	float cam_angle = 45.0;

//	vec3 l1_diffuse = vec3(0.0,1.0,0.0);
//	vec3 l1_specular = vec3(1.0,1.0,1.0);

//	vec3 l2_diffuse = vec3(0.0,0.0,1.0);
//	vec3 l2_specular = vec3(1.0,1.0,1.0);


	void onCreate() {

		camera = Camera(45.0, float(width)/float(height), 0.01, 100.0).translateZ(-5.0);
		vector<string> pic_names = {"indian.jpg", "hubble.jpg", "mask.jpg", "prism.jpg", "tile.jpg", "wolf.jpg", "indian.jpg", "mask.jpg", "prism.jpg"};
		textures.resize(pic_names.size());
		auto pic = pic_names.begin();
		for(auto & texture : textures) {
			
			rh.loadTexture(texture, *pic);
			texture.minFilter(GL_NEAREST);
			texture.maxFilter(GL_NEAREST);
			pic++;
			
		}

		rh.loadProgram(textureProgram, "texture", posLoc, -1, texCoordLoc, -1);
//		rh.loadProgram(phongProgram, "phong", posLoc, normalLoc, texCoordLoc, -1);
		rh.loadProgram(programColor, "color", posLoc, normalLoc, -1, colorLoc);

//		MeshData md1;
//		addCube(md1, true, 0.95);

		// makes 9 cubes with textures
//		vector<MeshData> md2(9);
//		for(auto & cube : md2) {
		cubeMeshBuffer1.resize(9);
		MeshData cube;
		addCube(cube, true, 0.1);
		for(auto & cubeMB : cubeMeshBuffer1)
			cubeMB.init(cube, posLoc, normalLoc, texCoordLoc, -1);
//		}

		// makes 9 cubes with different colors
		cubeMeshBuffer2.resize(9);
		MeshData cube2;
		addCube(cube2, 0.1); //this version makes normals, texcoords, and colors each side with a different default color
		for(auto & cubeMB : cubeMeshBuffer2)
			cubeMB.init(cube2, posLoc, normalLoc, -1, colorLoc);

		
		rotateBehavior = Behavior(now()).delay(1000).length(5000).range(vec3(180.0, 90.0, 360.0)).reversing(true).repeats(-1).sine();


//		proj = glm::perspective(45.0, 1.0, 0.1, 100.0);
//		view = glm::lookAt(vec3(0.0,0.0,5), vec3(0,0,0), vec3(0,1,0) );
		model = glm::mat4();

		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, width, height);
	}


	void draw(mat4& model, MeshBuffer& mb, Texture& t, Program& p) {

		p.bind(); {
		  glUniformMatrix4fv(p.uniform("model"), 1, 0, ptr(model));
		  glUniformMatrix4fv(p.uniform("view"), 1, 0, ptr(camera.view*view));
		  glUniformMatrix4fv(p.uniform("proj"), 1, 0, ptr(camera.projection));
		  
		  t.bind(GL_TEXTURE0); {
			
			glUniform1i(p.uniform("tex0"), 0);
			mb.draw();
			
		  } t.unbind(GL_TEXTURE0);
		  
		} p.unbind();
	}
		
	void draw(mat4 proj, mat4 view) {
		
//		handleKeys();
		handleMouse();

		model = glm::mat4(1.0);

		vec3 totals = rotateBehavior.tick(now()).totals();
		model = glm::rotate(model, totals.x, vec3(1.0f,0.0f,0.0f));
		model = glm::rotate(model, totals.y, vec3(0.0f,1.0f,0.0f));
		model = glm::rotate(model, totals.z, vec3(0.0f,0.0f,1.0f));


		//draw cube 1 into an offscreen texture
//		fbo.bind(); {
//		  glViewport(0, 0, width, height);
//		  glClearColor(0.1,0.1,0.1,1.0);
//		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		  
//		  draw(model, cubeMeshBuffer1, texture, textureProgram);
		  
//		} fbo.unbind();


//		glViewport(0, 0, width, height);
//		glClearColor(0.0,0.0,0.0,1.0);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double xpos = -1.0, zpos = 0.0;
		int i = 0;
//		int texture = 0;
//		cout << "pictured cube\n";
		for(auto & cubeMB : cubeMeshBuffer1) {

//			cout << "x position: " << xpos << " z position: " << zpos << endl;
			model = glm::mat4(1.0);

			model = glm::translate(model, vec3(xpos,0.0,zpos));
			model = glm::rotate(model, totals.x, vec3(1.0f,0.0f,0.0f));
			model = glm::rotate(model, totals.y, vec3(0.0f,1.0f,0.0f));
			model = glm::rotate(model, totals.z, vec3(0.0f,0.0f,1.0f));

			draw(model, cubeMB, textures[i], textureProgram);
			
			xpos += 0.25;
			if(i >= 4)
				zpos -= 0.25;
			else
				zpos += 0.25;
			++i;
			
		}

		//draw cube 3 - a colored cube
		xpos = 1.0;
		i = 0;
		zpos = -0.25;
//		cout << "random colored\n";
		for(auto & cubeMB : cubeMeshBuffer2) {
			
//			cout << "x position: " << xpos << " z position: " << zpos << endl;
			model = mat4(1.0);
			model = glm::translate(model, vec3(xpos,0.0,zpos));

			model = glm::rotate(model, -totals.x, vec3(1.0f,0.0f,0.0f));
			model = glm::rotate(model, -totals.y, vec3(0.0f,1.0f,0.0f));
			model = glm::rotate(model, -totals.z, vec3(0.0f,0.0f,1.0f));

			programColor.bind(); {
				
				glUniformMatrix4fv(programColor.uniform("model"), 1, 0, ptr(model));
				glUniformMatrix4fv(programColor.uniform("view"), 1, 0, ptr(view));
				glUniformMatrix4fv(programColor.uniform("proj"), 1, 0, ptr(proj));
			  
				cubeMB.draw();
			  
			} programColor.unbind();
			
			xpos -= 0.25;
			if(i >= 4)
				zpos += 0.25;
			else
				zpos -= 0.25;
			++i;
			
		}

	}
	
	void onFrame(){
		
		if (camera.isTransformed) { //i.e. if you've pressed any of the keys to move or rotate the camera around
			camera.transform();
		}
		
		
		glViewport(0, 0, width, height); {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			draw(camera.projection, camera.view*view);
		}
		
//		camera.printCameraInfo();
		
	}
	
	float dir = 0;
	void handleMouse() {
		
		if(isMoving) {

			if(previousMouseX-mouseX > 0)
				dir = -0.01;
			else
				dir = 0.01;
			
			view = glm::rotate(view, float(dir*mouseX), vec3(0.0, 1.0, 0.0));
			
		}
		isMoving = false;
		
	}
	
	virtual void keyDown(char key) {
		
		switch(key) {
			case kVK_Space :
				camera.resetVectors();
				break;
				
			case kVK_ANSI_A :
				camera.rotateY(2);
				break;
				
			case kVK_ANSI_D :
				camera.rotateY(-2);
				break;
				
			case kVK_ANSI_W :
				camera.rotateX(2);
				break;
				
			case kVK_ANSI_X :
				camera.rotateX(-2);
				break;
				
			case kVK_ANSI_E :
				camera.rotateZ(2);
				break;
				
			case kVK_ANSI_C :
				camera.rotateZ(-2);
				break;
				
			case kVK_ANSI_T :
				camera.translateZ(-0.5);
				break;
				
			case kVK_ANSI_B :
				camera.translateZ(0.5);
				break;
				
			case kVK_ANSI_Y :
				camera.translateX(0.5);
				break;
				
			case kVK_ANSI_N :
				camera.translateX(-0.5);
				break;
				
			case kVK_ANSI_U :
				camera.translateY(0.5);
				break;
				
			case kVK_ANSI_M :
				camera.translateY(-0.5);
				break;
				
			case kVK_ANSI_P :
				camera.printCameraInfo();
				break;
		}
	}

};

int main(){
	return FBOExample().start("aluminum::FBOExample", 100, 100, 400, 300);
}
