// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Include GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include "CornerTable.h"
// Include GLFW
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include "Utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

const float toRadians = 3.14159265f / 180.0f;

#include <glm/glm.hpp>
using namespace glm;


GLuint renderingProgram;

GLfloat* m_Vertices;
GLuint n_Vertices;
GLuint m_VBO;
GLuint m_VAO;
GLuint m_VAO1;
int dimVertices;
int numberOfVertices;

struct vertice{
	vec3 posicion;
	vec3 color;
};
int points;

float curAngle = 0.0f;

void init (GLFWwindow* window) {

	// Utils
	renderingProgram = Utils::createShaderProgram("src/vertShader.glsl", "src/fragShader.glsl");
	//Inicio de Corner table
	ifstream fin;
	fin.open("mesh/mesh24.mesh");
	string name; //Off
	int npoint,ntriangle,var3;//Numero de puntos, orden y colores
	int var0;;
	fin>>name;
	fin>>npoint>>ntriangle>>var3;

	//llenamos la lista de puntos
	double* vertexList = new double[3*npoint];
	for(int i = 0; i<npoint;++i){
		fin>>vertexList[3*i]>>vertexList[3*i+1]>>vertexList[3*i+2];
	}
	const CornerType numberTriangles = ntriangle;
	const CornerType numberVertices = npoint;
    CornerType* triangleList = new CornerType[3*ntriangle];
	for(int i = 0; i<ntriangle;++i){
		fin>>var0>>triangleList[i*3]>>triangleList[i*3+1]>>triangleList[i*3+2];
	}
	int numberCoordinatesByVertex = 3;
	CornerTable ct = CornerTable(triangleList,
								vertexList,
								numberTriangles,
								numberVertices,
								numberCoordinatesByVertex);
	//cout<<ct.getNumTriangles()<<endl<<ct.getNumberVertices()<<endl;
	vertice vortex[ct.getNumTriangles()*3];

	vec3 vert[ct.getNumTriangles()*3];
	//vec3 color[ct.getNumTriangles()*3]; // test (eliminar)
	//cout<<ct.getNumTriangles()<<endl;
	//cout<<ct.cornerToVertexIndex(9)<<endl; //Retorna posicion de un punto (no el punto)
	//cout<<vertexList[ct.cornerToVertexIndex(9)]<<endl; // Retorna un punto de la posicion
	int j = 0;
	for(int i = 0; i < ct.getNumTriangles();i++){
		vortex[i*3].posicion = vec3(vertexList[ct.cornerToVertexIndex(i*3+0)*3],vertexList[ct.cornerToVertexIndex(i*3+0)*3+1],vertexList[ct.cornerToVertexIndex(i*3+0)*3+2]);
		vortex[i*3+1].posicion = vec3(vertexList[ct.cornerToVertexIndex(i*3+1)*3],vertexList[ct.cornerToVertexIndex(i*3+1)*3+1],vertexList[ct.cornerToVertexIndex(i*3+1)*3+2]);
		vortex[i*3+2].posicion = vec3(vertexList[ct.cornerToVertexIndex(i*3+2)*3],vertexList[ct.cornerToVertexIndex(i*3+2)*3+1],vertexList[ct.cornerToVertexIndex(i*3+2)*3+2]);
		j++;

		vortex[i*3].color = vec3(j%1+0.23,j%3+0.24,j%5+0.1);
		vortex[i*3+1].color = vec3(j%1+0.23,j%3+0.24,j%5+0.1);
		vortex[i*3+2].color = vec3(j%1+0.23,j%3+0.24,j%5+0.1);
	}
	points = ct.getNumTriangles()*3;



	glGenBuffers(1, &m_VBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(vortex),	// tamaño de buffer
			vortex,
			GL_STATIC_DRAW
	);
	glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				sizeof(vertice),
				(GLvoid*)offsetof(vertice,posicion)
	);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
					1,
					3,
					GL_FLOAT,
					GL_FALSE,
					sizeof(vertice),
					(GLvoid*)offsetof(vertice,color)
	);

	glEnableVertexAttribArray(1);


	glBindVertexArray(0);

}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	GLuint uniformModel = glGetUniformLocation(renderingProgram, "model");

	curAngle += 1.5f;
	if (curAngle >= 360)
	{
		curAngle -= 360;
	}

	glm::mat4 model(1.0f);
	//Giro Antihorario
	model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//Usando UniformMatrix
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//Usando ProgramUniform
	glProgramUniformMatrix4fv(renderingProgram, uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES,0,points);

	glBindVertexArray(0);

}

int main(void) {
    if (!glfwInit()) {
    	exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Corner table", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
    	exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);

    init(window);

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
