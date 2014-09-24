#include "Aluminum/Includes.hpp"

#include "Aluminum/Behavior.hpp"
#include "Aluminum/Program.hpp"
#include "Aluminum/MeshBuffer.hpp"
#include "Aluminum/MeshData.hpp"
#include "Aluminum/Shapes.hpp"
#include "Aluminum/Camera.hpp"
#include "Aluminum/Utils.hpp"
#include "Aluminum/MeshUtils.hpp"
#include "Aluminum/ResourceHandler.h"

using namespace aluminum;

class Lighting : public RendererOSX {
public:
	
	ResourceHandler rh;
	
	Camera camera;
	Program phong_program;
	GLint posLoc = 0, normalLoc = 1;
	mat4 modelSun, modelEarth, modelMoon;
	MeshData mesh1, mesh2, mesh3, lightMesh;
	MeshBuffer mb1, mb2, mb3;
	
	Behavior rotateBehavior;
	
	
	vec3 ambient = vec3(0.1f,0.1f,0.1f);
	
	vec3 l1_diffuse = vec3(1.0f,1.0f,0.0f);
	vec3 l1_specular = vec3(1.0f,1.0f,1.0f);
	
	void onCreate() {
		
		rh.loadProgram(phong_program, "phong", posLoc, normalLoc, -1, -1);
		
		camera = Camera(90.0, (float)width/(float)height, 0.01, 100.0).translateZ(-10.0);
		
		addSphere(mesh1, 3.0, 100, 100);
		addSphere(mesh2, 1.0, 100, 100);
		addSphere(mesh3, 0.25, 100, 100);
		
		mb1.init(mesh1, posLoc, normalLoc, -1, -1);
		mb2.init(mesh2, posLoc, normalLoc, -1, -1);
		mb3.init(mesh3, posLoc, normalLoc, -1, -1);
		
		rotateBehavior = Behavior(now()).delay(0).range(vec3(0.0, 360.0, 0.0)).reversing(false).looping(true).repeats(-1);
		
		modelSun = glm::translate(mat4(), vec3(0,0,0));
		modelEarth = glm::translate(mat4(), vec3(5,0,0));
		modelMoon = glm::translate(mat4(), vec3(6,0,0));
		
		camera.printCameraInfo();
		
		glEnable(GL_DEPTH_TEST);
	}
	
	void draw(mat4 proj, mat4 view) {
		
		handleKeys();
		
		glViewport(0, 0, width, height);
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		vec4 l1_position = vec4(0.0,0.0,0.0,1.0);

		l1_diffuse = vec3(0.0,0.0,0.0);
		
		/* bind our Phong lighting shader */
		phong_program.bind(); {

			glUniformMatrix4fv(phong_program.uniform("view"), 1, 0, ptr(view));
			glUniformMatrix4fv(phong_program.uniform("proj"), 1, 0, ptr(proj));
			
			glUniform3fv(phong_program.uniform("ambient"), 1, ptr(ambient));
			
			glUniform4fv(phong_program.uniform("l1_position"), 1, ptr(l1_position));
			glUniform3fv(phong_program.uniform("l1_diffuse"), 1, ptr(l1_diffuse));
			glUniform3fv(phong_program.uniform("l1_specular"), 1, ptr(l1_specular));
			
			/* Draw the sun */
			modelSun = glm::translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
			
//			l1_diffuse = vec3(1.0,0.0,0.0);
//			l1_specular = vec3(0.0,0.0,0.0);
			
			glUniformMatrix4fv(phong_program.uniform("model"), 1, 0, ptr(modelSun));
			
//			glUniform3fv(phong_program.uniform("l1_diffuse"), 1, ptr(l1_diffuse));
//			glUniform3fv(phong_program.uniform("l1_specular"), 1, ptr(l1_specular));
			glUniform3fv(phong_program.uniform("ambient"), 1, ptr(vec3(1.0f,0.5f,0.1f)));
		
			mb1.draw();
			
			/* Draw the earth */
//			l1_diffuse = vec3(1.0,0.0,0.0);
//			l1_specular = vec3(0.0,0.0,1.0);
			
//			glUniform3fv(phong_program.uniform("l1_diffuse"), 1, ptr(l1_diffuse));
//			glUniform3fv(phong_program.uniform("l1_specular"), 1, ptr(l1_specular));
			glUniform3fv(phong_program.uniform("ambient"), 1, ptr(vec3(0.0f,1.0f,0.0f)));
			
			vec3 totals = rotateBehavior.tick(now()).totals();
			modelEarth = glm::rotate(modelSun, totals.y, vec3(0.0f,1.0f,0.0f));
			modelEarth = glm::translate(modelEarth, vec3(6.0f,0.0f,0.0f));
			glUniformMatrix4fv(phong_program.uniform("model"), 1, 0, ptr(modelEarth));
			mb2.draw();
			
			/* Draw the moon */
			l1_diffuse = vec3(1.0,0.0,0.0);
			
			glUniform3fv(phong_program.uniform("ambient"), 1, ptr(vec3(0.1f,0.1f,0.1f)));
			modelMoon = glm::rotate(modelEarth, totals.y, vec3(0.0f,1.0f, 0.0f));
			modelMoon = glm::translate(modelMoon, vec3(2.0f,0.0f,0.0f));
			glUniformMatrix4fv(phong_program.uniform("model"), 1, 0, ptr(modelMoon));
			mb3.draw();
			
		} phong_program.unbind();
		
	}
	
	
	void onFrame(){
		
		if (camera.isTransformed) {
			camera.transform();
		}
		
		
		glViewport(0, 0, width, height); {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			draw(camera.projection, camera.view);
		}
		
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
		}
	}
};

int main(){
	return Lighting().start("aluminum::Lighting", 100, 100, 800, 600);
}
