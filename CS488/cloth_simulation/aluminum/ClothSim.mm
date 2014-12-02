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

/* Some physics constants */
#define DAMPING 0.01 // how much to damp the cloth simulation each frame
#define TIME_STEPSIZE2 0.5*0.5 // how large time step each particle takes each frame
#define CONSTRAINT_ITERATIONS 15 // how many iterations of constraint satisfaction each frame (more is rigid, less is soft)

using namespace aluminum;

class Particle {

private:
	bool movable;
	float mass;
	vec3 pos;
	vec3 old_pos;

	vec3 acceleration;
	vec3 accumulated_normal;

public:
	Particle(vec3 pos) : pos(pos), old_pos(pos),acceleration(vec3(0,0,0)), mass(1), movable(true), accumulated_normal(vec3(0,0,0)){}
	Particle(){}

	void addForce(vec3 f) {
		acceleration += f/mass;
	}

	/* Given the equation force = mass * acceleration, the next position is
	found through verlet integration */
	void timeStep() {
		if(movable) {
			vec3 temp = pos;
			pos = pos + (pos-old_pos)*(float)(1.0f-DAMPING) + acceleration*(float)(TIME_STEPSIZE2);
			old_pos = temp;
			resetAcceleration(); // acceleration is reset since it HAS been translated into a change in position (and implicitely into velocity)
		}
	}

	vec3& getPos() {return pos;}

	void resetAcceleration() {acceleration = vec3(0,0,0);}

	void offsetPos(const vec3 v) {if(movable) pos += v;}

	void makeUnmovable() {movable = false;}

	void addToNormal(vec3 normal) {
		accumulated_normal += glm::normalize(normal);
	}

	vec3& getNormal() { return accumulated_normal;} // normal is not unit length

	void resetNormal() {accumulated_normal = vec3(0,0,0);}
};

class Constraint {

private:
	float rest_distance;

public:
	Particle *p1, *p2; // the two particles that are connected through this constraint

	Constraint(Particle *p1, Particle *p2) :  p1(p1),p2(p2)
	{
		vec3 vec = p1->getPos()-p2->getPos();
		rest_distance = glm::length(vec);
	}

	/* This is one of the important methods, where a single constraint between two particles p1 and p2 is solved
	the method is called by Cloth.time_step() many times per frame*/
	void satisfyConstraint()
	{
		vec3 p1_to_p2 = p2->getPos() - p1->getPos(); // vector from p1 to p2
		float current_distance = glm::length(p1_to_p2); // current distance between p1 and p2
		vec3 correctionVector = p1_to_p2*(1 - rest_distance/current_distance); // The offset vector that could moves p1 into a distance of rest_distance to p2
		vec3 correctionVectorHalf = correctionVector*0.5f; // Lets make it half that length, so that we can move BOTH p1 and p2.
		p1->offsetPos(correctionVectorHalf); // correctionVectorHalf is pointing from p1 to p2, so the length should move p1 half the length needed to satisfy the constraint.
		p2->offsetPos(-correctionVectorHalf); // we must move p2 the negative direction of correctionVectorHalf since it points from p2 to p1, and not p1 to p2.
	}

};

class Cloth
{
private:
	struct Vertex {
		vec3 position;
		vec3 color;
		vec3 normal;
	};

	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	// total number of particles is num_particles_width*num_particles_height

	std::vector<Particle> particles; // all particles that are part of this cloth
	std::vector<Constraint> constraints; // all constraints between particles as part of this cloth

	Particle* getParticle(int x, int y) {return &particles[y*num_particles_width + x];}
	void makeConstraint(Particle *p1, Particle *p2) {constraints.push_back(Constraint(p1,p2));}


	/* A private method used by drawShaded() and addWindForcesForTriangle() to retrieve the
	normal vector of the triangle defined by the position of the particles p1, p2, and p3.
	The magnitude of the normal vector is equal to the area of the parallelogram defined by p1, p2 and p3
	*/
	vec3 calcTriangleNormal(Particle *p1,Particle *p2,Particle *p3)
	{
		vec3 pos1 = p1->getPos();
		vec3 pos2 = p2->getPos();
		vec3 pos3 = p3->getPos();

		vec3 v1 = pos2-pos1;
		vec3 v2 = pos3-pos1;

		return glm::cross(v1, v2);
	}

	/* A private method used by windForce() to calcualte the wind force for a single triangle
	defined by p1,p2,p3*/
	void addWindForcesForTriangle(Particle *p1,Particle *p2,Particle *p3, const vec3 direction)
	{
		vec3 normal = calcTriangleNormal(p1,p2,p3);
		vec3 d = glm::normalize(normal);
		vec3 force = normal * (glm::dot(d, direction));
		p1->addForce(force);
		p2->addForce(force);
		p3->addForce(force);
	}

	/* A private method used by drawShaded(), that draws a single triangle p1,p2,p3 with a color*/
	void drawTriangle(Particle *p1, Particle *p2, Particle *p3, const vec3 color, std::vector<Vertex> &vertexData)
	{
//		glColor3fv( (GLfloat*) &color);

		// vec3 temp = normalize(p1->getNormal());
		// glNormal3fv((GLfloat *) &temp);
		// glVertex3fv((GLfloat *) &(p1->getPos()));
		// temp = normalize(p2->getNormal());
		// glNormal3fv((GLfloat *) &temp);
		// glVertex3fv((GLfloat *) &(p2->getPos()));
		// temp = normalize(p3->getNormal());
		// glNormal3fv((GLfloat *) &temp);
		// glVertex3fv((GLfloat *) &(p3->getPos()));

		// works with open gl 3.2
		Vertex v1 = {p1->getPos(), color, p1->getNormal()};
		vertexData.push_back(v1);
		Vertex v2 = {p2->getPos(), color, p2->getNormal()};
		vertexData.push_back(v2);
		Vertex v3 = {p3->getPos(), color, p3->getNormal()};
		vertexData.push_back(v3);
	}

public:

	/* This is a important constructor for the entire system of particles and constraints*/
	Cloth(float width, float height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
	{
		particles.resize(num_particles_width*num_particles_height); //I am essentially using this vector as an array with room for num_particles_width*num_particles_height particles

		// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
		for(int x=0; x<num_particles_width; x++)
		{
			for(int y=0; y<num_particles_height; y++)
			{
				vec3 pos = vec3(width * (x/(float)num_particles_width),
								-height * (y/(float)num_particles_height),
								0);
				particles[y*num_particles_width+x]= Particle(pos); // insert particle in column x at y'th row
			}
		}

		// Connecting immediate neighbor particles with constraints (distance 1 and sqrt(2) in the grid)
		for(int x=0; x<num_particles_width; x++)
		{
			for(int y=0; y<num_particles_height; y++)
			{
				if (x<num_particles_width-1) makeConstraint(getParticle(x,y),getParticle(x+1,y));
				if (y<num_particles_height-1) makeConstraint(getParticle(x,y),getParticle(x,y+1));
				if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getParticle(x,y),getParticle(x+1,y+1));
				if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getParticle(x+1,y),getParticle(x,y+1));
			}
		}


		// Connecting secondary neighbors with constraints (distance 2 and sqrt(4) in the grid)
		for(int x=0; x<num_particles_width; x++)
		{
			for(int y=0; y<num_particles_height; y++)
			{
				if (x<num_particles_width-2) makeConstraint(getParticle(x,y),getParticle(x+2,y));
				if (y<num_particles_height-2) makeConstraint(getParticle(x,y),getParticle(x,y+2));
				if (x<num_particles_width-2 && y<num_particles_height-2) makeConstraint(getParticle(x,y),getParticle(x+2,y+2));
				if (x<num_particles_width-2 && y<num_particles_height-2) makeConstraint(getParticle(x+2,y),getParticle(x,y+2));			}
		}


		// making the upper left most three and right most three particles unmovable
		for(int i=0;i<3; i++)
		{
			getParticle(0+i ,0)->offsetPos(vec3(0.5,0.0,0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			getParticle(0+i ,0)->makeUnmovable();

			getParticle(0+i ,0)->offsetPos(vec3(-0.5,0.0,0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			getParticle(num_particles_width-1-i ,0)->makeUnmovable();
		}
	}

	/* drawing the cloth as a smooth shaded (and colored according to column) OpenGL triangular mesh
	Called from the display() method
	The cloth is seen as consisting of triangles for four particles in the grid as follows:

	(x,y)   *--* (x+1,y)
	        | /|
	        |/ |
	(x,y+1) *--* (x+1,y+1)

	*/
	void drawShaded(GLuint litShader, mat4 view, mat4 projection)
	{
		// reset normals (which where written to last frame)
		std::vector<Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++)
		{
			(*particle).resetNormal();
		}

		//create smooth per particle normals by adding up all the (hard) triangle normals that each particle is part of
		for(int x = 0; x<num_particles_width-1; x++)
		{
			for(int y=0; y<num_particles_height-1; y++)
			{
				vec3 normal = calcTriangleNormal(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1));
				getParticle(x+1,y)->addToNormal(normal);
				getParticle(x,y)->addToNormal(normal);
				getParticle(x,y+1)->addToNormal(normal);

				normal = calcTriangleNormal(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1));
				getParticle(x+1,y+1)->addToNormal(normal);
				getParticle(x+1,y)->addToNormal(normal);
				getParticle(x,y+1)->addToNormal(normal);
			}
		}

		static GLuint vertexArrayObject = 0;
		static GLuint vertexBuffer = 0;
		if (vertexArrayObject == 0){
			glGenVertexArrays(1, &vertexArrayObject);
			glBindVertexArray(vertexArrayObject);

			glGenBuffers(1, &vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

			GLuint positionAttributeLocation = glGetAttribLocation(litShader, "position");
			GLuint colorAttributeLocation = glGetAttribLocation(litShader, "color");
			GLuint normalAttributeLocation = glGetAttribLocation(litShader, "normal");
			glEnableVertexAttribArray(positionAttributeLocation);
			glEnableVertexAttribArray(colorAttributeLocation);
			glEnableVertexAttribArray(normalAttributeLocation);
			glVertexAttribPointer(positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
			glVertexAttribPointer(colorAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)sizeof(vec3));
			glVertexAttribPointer(normalAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(sizeof(vec3)*2));
		}
		std::vector<Vertex> vertexData;

		for(int x = 0; x<num_particles_width-1; x++)
		{
			for(int y=0; y<num_particles_height-1; y++)
			{
				vec3 color(0,0,0);
				if (x%2) // red and white color is interleaved according to which column number
					color = vec3(0.6f,0.2f,0.2f);
				else
					color = vec3(1.0f,1.0f,1.0f);

				drawTriangle(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1),color, vertexData);
				drawTriangle(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1),color, vertexData);
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), ptr(vertexData[0].position), GL_STREAM_DRAW);
		mat4 modelView = view;
		mat4 mvp = projection * modelView;
		glUniformMatrix4fv(glGetUniformLocation(litShader, "mvp"),1,false, ptr(mvp));
		mat4 normalMatrix = glm::inverse(glm::transpose(modelView));
		glUniformMatrix3fv(glGetUniformLocation(litShader, "normalMatrix"),1,false, ptr(normalMatrix));

		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_TRIANGLES, 0, vertexData.size());
		glUseProgram(0);
	}

	/* this is an important methods where the time is progressed one time step for the entire cloth.
	This includes calling satisfyConstraint() for every constraint, and calling timeStep() for all particles
	*/
	void timeStep()
	{
		std::vector<Constraint>::iterator constraint;
		for(int i=0; i<CONSTRAINT_ITERATIONS; i++) // iterate over all constraints several times
		{
			for(constraint = constraints.begin(); constraint != constraints.end(); constraint++ )
			{
				(*constraint).satisfyConstraint(); // satisfy constraint.
			}
		}

		std::vector<Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++)
		{
			(*particle).timeStep(); // calculate the position of each particle at the next time step.
		}
	}

	/* used to add gravity (or any other arbitrary vector) to all particles*/
	void addForce(const vec3 direction)
	{
		std::vector<Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++)
		{
			(*particle).addForce(direction); // add the forces to each particle
		}

	}

	/* used to add wind forces to all particles, is added for each triangle since the final force is proportional to the triangle area as seen from the wind direction*/
	void windForce(const vec3 direction)
	{
		for(int x = 0; x<num_particles_width-1; x++)
		{
			for(int y=0; y<num_particles_height-1; y++)
			{
				addWindForcesForTriangle(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1),direction);
				addWindForcesForTriangle(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1),direction);
			}
		}
	}

	/* used to detect and resolve the collision of the cloth with the ball.
	This is based on a very simples scheme where the position of each particle is simply compared to the sphere and corrected.
	This also means that the sphere can "slip through" if the ball is small enough compared to the distance in the grid bewteen particles
	*/
	void ballCollision(const vec3 center,const float radius )
	{
		std::vector<Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++)
		{
			vec3 v = (*particle).getPos()-center;
			float l = glm::length(v);
			if ( glm::length(v) < radius) // if the particle is inside the ball
			{
				(*particle).offsetPos(glm::normalize(v)*(radius-l)); // project the particle to the surface of the ball
			}
		}
	}

	void doFrame()
	{

	}

};

class ClothSim : public RendererOSX {

public:

//	Cloth cloth(14,10,55,45); // one Cloth object of the Cloth class
	Cloth * cloth1 = new Cloth(10,10,10,10);
	vec3 ball_pos = vec3(7,-5,0); // the center of our one ball
	float ball_radius = 2; // the radius of our one ball
	float ball_time = 0;

	// lighting vectors
	// vec4 lightModelAmbient = vec4(0.2, 0.2, 0.2, 1.0);

	vec4 lightPos0 = vec4(-1.0,1.0,0.5,0.0);
	vec4 lightAmbient0 = vec4(0.2, 0.2, 0.2, 1.0);
	vec4 lightDiffuse0 = vec4(0.8, 0.8, 0.8, 1.0);

	vec4 lightPos1 = vec4(1.0,0.0,-0.2,0.0);
	vec4 lightAmbient1 = vec4(0.0,0.0,0.0,0.0);
	vec4 lightDiffuse1 = vec4(0.5,0.5,0.3,0.0);

	vec4 ambient[2] = {lightAmbient0, lightAmbient1};
	vec4 diffuse[2] = {lightDiffuse0, lightDiffuse1};

	// models for camera and ball and cloth
	mat4 model, view, projection, ball_model;
	// data for the ball mesh and ball buffer
	MeshData ballMesh;
	MeshBuffer ballBuffer;
	// camera and shaders to use
	ResourceHandler rh;
	Camera camera;
	Program litProgram, unlitProgram;
	GLint posLoc = 0, normalLoc = 1, colorLoc = 3;
	GLuint litShader, unlitShader;

	void onCreate() {

		// cout << "wtf!!!" << endl;
		rh.loadProgram(litProgram, "litShader", posLoc, normalLoc, -1, 3);
		rh.loadProgram(unlitProgram, "unlit", posLoc, normalLoc, -1, -1);
		// cout << "Yes!!!" << endl;
		litShader = litProgram.id();
		unlitShader = unlitProgram.id();

		glUseProgram(litShader);
		glUniform4fv(glGetUniformLocation(litShader, "lightAmbient"),2, ptr(ambient[0]));
		glUniform4fv(glGetUniformLocation(litShader, "lightDiffuse"),2, ptr(diffuse[0]));

		vec4 lightModelAmbient = vec4(0.2, 0.2, 0.2, 1.0);
		glUniform4fv(glGetUniformLocation(litShader, "lightModelAmbient"),1, ptr(lightModelAmbient));
		glUseProgram(0);
//		 litShader = loadShader("../resources/litShader.vsh", "../resources/litShader.fsh");
//		 unlitShader = loadShader("../resources/unlit.vsh", "../resources/unlit.fsh");

		projection = glm::perspective(80.0f,( float )width /( float )height, 1.0f, 5000.0f);
//		glShadeModel(GL_SMOOTH);
		glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

	}

	void drawScreenQuad(GLuint unlitShader) {
		glUseProgram(unlitShader);
		static GLuint vertexArrayObject = 0;
		if (vertexArrayObject == 0){
			struct Vertex {
				vec3 position;
				vec3 color;
			};
			Vertex vertexData[4] = {
				{vec3(-200.0f,-100.0f,-100.0f ), vec3( 0.8f,0.8f,1.0f )},
				{vec3( 200.0f,-100.0f,-100.0f ), vec3( 0.8f,0.8f,1.0f )},
				{vec3(-200.0f, 100.0f,-100.0f ), vec3( 0.4f,0.4f,0.8f )},
				{vec3( 200.0f, 100.0f,-100.0f ), vec3( 0.4f,0.4f,0.8f )},

			};

			glGenVertexArrays(1, &vertexArrayObject);
			glBindVertexArray(vertexArrayObject);

			GLuint vertexBuffer;
			glGenBuffers(1, &vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), ptr(vertexData[0].position), GL_STATIC_DRAW);

			GLuint positionAttributeLocation = glGetAttribLocation(unlitShader, "position");
			GLuint colorAttributeLocation = glGetAttribLocation(unlitShader, "color");
			glEnableVertexAttribArray(positionAttributeLocation);
			glEnableVertexAttribArray(colorAttributeLocation);
			glVertexAttribPointer(positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
			glVertexAttribPointer(colorAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)sizeof(vec3));
		}
		glUniformMatrix4fv(glGetUniformLocation(unlitShader, "mvp"),1,false, ptr(projection));
		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glUseProgram(0);
	}

	void drawSolidSphere(vec3& position, GLuint litShader){
		glUseProgram(litShader);
		static GLuint vertexArrayObject = 0;
		static int elementCount;
		if (vertexArrayObject == 0){
			struct Vertex {
				vec3 position;
				vec3 color;
				vec3 normal;
			};
			vec3 color = vec3(0.4f,0.8f,0.5f);
			std::vector<Vertex> vertexData;
			int slices = 64;
	        int stacks = 32;
	        float radius = 1.9;
	        int vertexCount = (stacks+1) * (slices + 1);
	        float piDivStacks = M_PI / stacks;
	        float PIDiv2 = M_PI / 2;
	        float PI2 = M_PI * 2;

			for (int j = 0; j <= stacks; j++) {
				float latitude1 = piDivStacks * j - PIDiv2;
	            float sinLat1 = sin(latitude1);
	            float cosLat1 = cos(latitude1);
	            for (int i = 0; i <= slices; i++) {
	                float longitude = (PI2 / slices) * i;
	                float sinLong = sin(longitude);
	                float cosLong = cos(longitude);
					vec3 normal = vec3(cosLong * cosLat1, sinLat1, sinLong * cosLat1);
					vec3 position = normal * radius;
					Vertex v = {position, color, normal};
					vertexData.push_back(v);
	            }
	        }
			std::vector<GLuint> indices;
	        // create indices
	        for (int j = 0; j < stacks; j++) {
				int index;
	            if (j > 0) {
	                indices.push_back(j * (slices + 1)); // make degenerate
	            }
	            for (int i = 0; i <= slices; i++) {
	                index = j * (slices + 1) + i;
	                indices.push_back(index);
	                indices.push_back(index + slices + 1);
	            }
	            if (j + 1 < stacks) {
	                indices.push_back(index + slices + 1); // make degenerate
	            }
	        }

			glGenVertexArrays(1, &vertexArrayObject);
			glBindVertexArray(vertexArrayObject);

			GLuint vertexBuffer;
			glGenBuffers(1, &vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), ptr(vertexData[0].position), GL_STATIC_DRAW);

			GLuint positionAttributeLocation = glGetAttribLocation(litShader, "position");
			GLuint colorAttributeLocation = glGetAttribLocation(litShader, "color");
			GLuint normalAttributeLocation = glGetAttribLocation(litShader, "normal");
			glEnableVertexAttribArray(positionAttributeLocation);
			glEnableVertexAttribArray(colorAttributeLocation);
			glEnableVertexAttribArray(normalAttributeLocation);
			glVertexAttribPointer(positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
			glVertexAttribPointer(colorAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)sizeof(vec3));
			glVertexAttribPointer(normalAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(sizeof(vec3)*2));

			GLuint elementArrayBuffer;
			glGenBuffers(1, &elementArrayBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &(indices[0]), GL_STATIC_DRAW);
			elementCount = indices.size();

		}

		mat4 modelView = view;
		modelView = glm::translate(modelView, position);
		mat4 mvp = projection * modelView;
		glUniformMatrix4fv(glGetUniformLocation(litShader, "mvp"),1,false, ptr(mvp));
		mat4 normalMatrix = glm::inverse(glm::transpose(modelView));
		glUniformMatrix3fv(glGetUniformLocation(litShader, "normalMatrix"),1,false, ptr(normalMatrix));

		glBindVertexArray(vertexArrayObject);
		glDrawElements(GL_TRIANGLE_STRIP, elementCount, GL_UNSIGNED_INT, 0);
		glUseProgram(0);
	}

	void onFrame() {

		// projection = glm::perspective(80.0f,( float )width /( float )height, 1.0f, 5000.0f);
//		ball_time++;
//		ball_pos[2] = (float)cos(ball_time/50.0f)*7;

//		cloth1->addForce(vec3(0,-0.2,0)*(float)(TIME_STEPSIZE2)); // add gravity each frame, pointing down
//		cloth1->windForce(vec3(0.5,0,0.2)*(float)(TIME_STEPSIZE2)); // generate some wind each frame
//		cloth1->timeStep(); // calculate the particle positions of the next frame
//		cloth1->ballCollision(ball_pos,ball_radius); // resolve collision with the ball

		// drawing
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		drawScreenQuad(unlitShader); // drawing some smooth shaded background - because I like it ;)
		view = mat4(1.0f);
		view = glm::translate(view, vec3(-6.5, 6, -9.0f));
		view = glm::rotate(view, 25.0f, vec3(0,1,0));

		// setup light
		glUseProgram(litShader);
		vec4 eyeSpaceLight[2] = { lightPos0, lightPos1};
		glUniform4fv(glGetUniformLocation(litShader, "lightPosition"), 2, ptr(eyeSpaceLight[0]));
		glUseProgram(0);

		cloth1->drawShaded(litShader, view, projection);
		drawSolidSphere(ball_pos, litShader);

	}

};

int main(){
  return ClothSim().start("aluminum::Cloth Simulation", 100, 100, 1280, 720);
}
