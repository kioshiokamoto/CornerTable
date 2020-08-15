//============================================================================
//Modificar init-display para dibujar corner table
//============================================================================

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <fstream>

#include "DefinitionTypes.h"


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
using namespace glm;
#include "CornerTable.h"
// Include GLEW
#define GLEW_STATIC
#include <GL\glew.h>

// Include GLFW
#include <GLFW\glfw3.h>

using namespace std;

// GLuint : unsigned int
GLuint renderingProgram;

GLuint m_VBO;
GLuint m_VAO;
int points;
using namespace std;
struct vortex{
	vec3 pos;
};

GLuint createShaderProgram() {
    // declares two shaders as character strings
    // Vertex Shader code
    static const char *vshaderSource =
    		"#version 330  \n"
    		"layout (location = 0) in vec3 pos; \n"
    		"void main() { \n"
    		" gl_Position = vec4(0.3*pos.x, 0.3*pos.y, pos.z, 1.0);	\n"
    		"}";

    // Fragment Shader
    static const char *fshaderSource =
    		"#version 330  \n"
    		"out vec4 colour; \n"
    		"void main(){ \n"
    		" colour = vec4(1.0, 0.0, 0.0, 1.0); \n"
    		"}";
    
    // glCreateShader : generates the two shaders of types GL_VERTEX_SHADER and GL_FRAGMENT_SHADER
    // OpenGL creates each shader object, and returns an integer ID for each
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // loads the GLSL code from the strings into the empty shader objects
    // glShaderSource(ShaderObject, NumberOfStrings : 1, ArrayOfPointers, -)
    glShaderSource(vShader, 1, &vshaderSource, NULL);
    glShaderSource(fShader, 1, &fshaderSource, NULL);
    
    // the shaders are each compiled
    glCompileShader(vShader);
    glCompileShader(fShader);
    
    // the integer ID of a program object
    GLuint vfProgram = glCreateProgram();
    
    // attatches each of the shaders to the program object
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    
    // requests that the GLSL compiler ensure that the shaders are compatible
    glLinkProgram(vfProgram);
    
    return vfProgram;
}

void init (GLFWwindow* window) {
    renderingProgram = createShaderProgram();
    ifstream fin;
        	fin.open("mesh/mesh15.mesh");

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
        	CornerTable ct = CornerTable(triangleList,vertexList,
        										numberTriangles,
        										numberVertices,
        										numberCoordinatesByVertex);

        	//ct._cornerToVertex
    //cout<<ct.getNumTriangles()<<endl<<ct.getNumberVertices()<<endl;

    vec3 vert[ct.getNumTriangles()*3];


    //cout<<ct.getNumTriangles()<<endl;

    //cout<<ct.cornerToVertexIndex(9)<<endl; //Retorna posicion de un punto (no el punto)

    //cout<<vertexList[ct.cornerToVertexIndex(9)]<<endl; // Retorna un punto de la posicion

    for(int i = 0; i < ct.getNumTriangles();i++){
    	vert[i*3] = vec3(vertexList[ct.cornerToVertexIndex(i*3+0)*3],vertexList[ct.cornerToVertexIndex(i*3+0)*3+1],vertexList[ct.cornerToVertexIndex(i*3+0)*3+2]);
    	vert[i*3+1]=vec3(vertexList[ct.cornerToVertexIndex(i*3+1)*3],vertexList[ct.cornerToVertexIndex(i*3+1)*3+1],vertexList[ct.cornerToVertexIndex(i*3+1)*3+2]);
		vert[i*3+2]=vec3(vertexList[ct.cornerToVertexIndex(i*3+2)*3],vertexList[ct.cornerToVertexIndex(i*3+2)*3+1],vertexList[ct.cornerToVertexIndex(i*3+2)*3+2]);
    }



    points = ct.getNumTriangles()*3;


	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	glVertexAttribPointer(
			0,	// Atributo 0 (layout = 0 )
			3,	// cada vertice posee 3 valores
			GL_FLOAT,	// cada valor del vértice es del tipo GLfloat
			GL_FALSE,	// Quer normalizar os dados e converter tudo pra NDC ? ( no nosso caso, já esta tudo correto, então deixamos como FALSE)
			0,	// De quantos em quantos bytes, este atributo é encontrado no buffer
			0	// Onde está o primeiro valor deste atributo no buffer.
	);
	glEnableVertexAttribArray(0);	// Habilita este atributo "0"

	glBindBuffer(GL_ARRAY_BUFFER, 0);
    // VAO : Vertex Array Objects, OpenGL requires at least one VAO
    // Cria um ID na GPU para um array de  buffers
	glBindVertexArray(m_VAO);
}

void display(GLFWwindow* window, double currentTime) {
    // loads the program containing the two compiled shaders into the OpenGL pipeline stages (onto the GPU)
    glUseProgram(renderingProgram);
    //glPointSize(30.0f);
    // initiates pipeline processing
    // mode: GL_POINTS, from 0, one (point)
    glDrawArrays(GL_TRIANGLES, 0, points);
}

int main(void) {
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 	// Resizable option.

    GLFWwindow* window = glfwCreateWindow(800, 800, "Corner Table", NULL, NULL);
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
