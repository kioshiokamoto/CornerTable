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

#include "DefinitionTypes.h"
#include "min_path.h"



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

const float toRadians = 3.14159265f / 180.0f;

#include <glm/glm.hpp>
using namespace glm;
#include <time.h>

#define numVAOs 1
#define numVBOs 1
#define numEBOs 2
#define normalize 2

GLuint renderingProgram;

GLfloat* m_Vertices;
GLuint n_Vertices;
GLuint m_VBO;
GLuint m_EBO;
GLuint m_VAO;
int dimVertices;
int numberOfVertices;



GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint ebo[numEBOs];



CornerTable *ct;
int *indices;
vector<unsigned int> path_indices;

struct vertice{
	vec3 posicion;
	vec3 color;

};
int points;

float curAngle = 0.0f;

void pointGenerate(double &x1, double &y1,  double &x2 ,double &y2)
{
/*
    double a1 = ((rand() % 1000001) / 1000000.0) / 5.0;
    double a2 = ((rand() % 1000001) / 1000000.0) / 5.0;
    double a3 = ((rand() % 1000001) / 1000000.0) / 5.0;
    double a4 = ((rand() % 1000001) / 1000000.0) / 5.0;
    double a5 = ((rand() % 1000001) / 1000000.0) / 5.0;
    double a6 = 1.0 - (a1 + a2 + a3 + a4 + a5);

    x = a1 * (2) + a2 * (1) + a3 * (-1) + a4 * (-2) + a5 * (-1) + a6 * (1);
    y = a1 * (0) + a2 * (1.73205) + a3 * (1.73205) + a4 * (0) + a5 * (-1.73205) + a6 * (-1.73205); */

    srand( time( NULL ) );

    x1 = -3.0000 + (rand( ) % 60010 ) / 10000.0f;
    y1= -3.0000 + (rand( ) % 60010 ) / 10000.0f;
    x2 = -3.0000 + (rand( ) % 60010 ) / 10000.0f;
    y2= -3.0000 + (rand( ) % 60010 ) / 10000.0f;

}

void ubicarCoordenada(double vertexList[],int npoint,double punto1x, double punto1y,int &punto){
	//(npoint-1)*3  = x
	//(npoint-1)*3+1  = y
	double tempx,tempy;
	double cercanox,cercanoy,puntoCercano;
	double cercanoxComparador,cercanoyComparador;
	cercanoxComparador = 1000;
	cercanoyComparador = 1000;
		//Guardar punto cercano, despues recorrer la lista de puntos de 3 en 3 y al encontrar el primero pintar triangulo de blanco.
		//O  guardar punto cercano e intentar usar metodo ct.getCornerNeighbours();
		// punto1x,punto1y,punto2x,punto2y
		//0.979149   -1.70024

	for(int i = 1 ; i <= npoint ;i++){
		tempx = vertexList[(i-1)*3];
		tempy = vertexList[(i-1)*3+1];
		//cout<< "[ x  : "<< tempx << " |  y : "<< tempy<<" ]"<<endl;
		//Se muestra la distancia entre los oduble
		cercanox = tempx - punto1x;
		if(cercanox<=0){
			cercanox = cercanox*(-1.0);
		}
		//cout<< "Coordenada x: "<<tempx<<"  \ Cercano x: "<<cercanox<<endl;
		cercanoy = tempy - punto1y;
		if(cercanoy<=0){
			cercanoy = cercanoy*(-1.0);
		}
		//cout<< "Coordenada y: "<<tempy<<"  \ Cercano y: "<<cercanoy<<endl;
		if(((punto1x - tempx) == 0) && ((punto1y - tempy) == 0)){
			punto = i-1; //validar
			break;
		}else{
			if(cercanox<cercanoxComparador && cercanoy < cercanoyComparador){
				cercanoxComparador = cercanox;
				cercanoyComparador = cercanoy;
				punto = i-1;
			}
		}
	}
}



int cantidadPuntos;
double* vertexList1;
void leerMesh(){
	ifstream fin;
	fin.open("mesh/mesh8.mesh");
	string name; //Off
	int npoint,ntriangle,var3;//Numero de puntos, orden y colores
	int var0;;
	fin>>name;
	fin>>npoint>>ntriangle>>var3;

	//Llenamos la lista de puntos
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
	CornerTable *CT = new CornerTable(triangleList,
									vertexList,
									numberTriangles,
									numberVertices,
									numberCoordinatesByVertex);
	ct = CT;
	cantidadPuntos = npoint;
	vertexList1= vertexList;

}

void buildPath(int o, int d){
    vector<unsigned int> path;
    const CornerType *trianglesPositions = ct->getTriangleList();

    min_path(path, ct, o, d);

    PrintDijkstraPath(path, o ,d);

    int aux = path[d];
    path_indices.push_back(trianglesPositions[d*3]);
    path_indices.push_back(trianglesPositions[d*3 + 1]);
    path_indices.push_back(trianglesPositions[d*3 + 2]);

    while(aux>-1){
        path_indices.push_back(trianglesPositions[aux*3]);
        path_indices.push_back(trianglesPositions[aux*3 + 1]);
        path_indices.push_back(trianglesPositions[aux*3 + 2]);
        aux = path[aux];
    }

    //for(unsigned int i=0; i<path_indices.size(); i+=3){
    //cout<<"I"<<" ::: "<<path_indices[i]<<","<<path_indices[i+1]<<","<<path_indices[i+2]<<endl;
     //}
}
void setupVertices() {

    double* vertexPositions = ct->getAttributes();
    const CornerType *trianglesPositions = ct->getTriangleList();

    for (unsigned int i=0; i<ct->getNumberVertices()*3; i++) // normalizar coordenadas
        vertexPositions[i] *= normalize;

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(numVBOs, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, ct->getNumberVertices()*3*sizeof(double), &vertexPositions[0], GL_STATIC_DRAW);

    glGenBuffers(numEBOs, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ct->getNumTriangles()*3*sizeof(CornerType), &trianglesPositions[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, path_indices.size()*3*sizeof(CornerType), &path_indices[0], GL_STATIC_DRAW);
}

void init (GLFWwindow* window) {
	//Generacion de puntos
	glUseProgram(renderingProgram);



	// Shaders
	renderingProgram = Utils::createShaderProgram("src/vertShader.glsl", "src/fragShader.glsl");

	leerMesh();


	double punto1x,punto1y,punto2x,punto2y;
	pointGenerate(punto1x,punto1y,punto2x,punto2y);
	int puntoCercano1,puntoCercano2;

	ubicarCoordenada(vertexList1,cantidadPuntos,punto1x,punto1y,puntoCercano1);
	ubicarCoordenada(vertexList1,cantidadPuntos,punto2x,punto2y,puntoCercano2);

	cout<<"[" << punto1x <<" - " <<punto1y<< "]"<< endl;
	cout<<"El punto mas cercano a la coordenada es "<<puntoCercano1<<endl;

	cout<<"[" << punto2x <<" - " <<punto2y<< "]"<< endl;
	cout<<"El punto mas cercano a la coordenada es "<<puntoCercano2<<endl;

	buildPath(puntoCercano1,puntoCercano2);

	int indice[ct->getNumTriangles()*3];
	double posicion[ct->getNumTriangles()*3];

	double color[ct->getNumTriangles()*3];


	//vertice vortex[ct->getNumTriangles()];
	//int j = 0;
	//cout<<ct->getNumTriangles()<<endl;


	for(unsigned int i=0; i<path_indices.size(); i+=3){
	    cout<<"I"<<" ::: "<<path_indices[i]<<","<<path_indices[i+1]<<","<<path_indices[i+2]<<endl;
	 }
	//indices = indice;

	setupVertices();

}

void display(GLFWwindow* window, double currentTime) {

	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	//glBindVertexArray(m_VAO);
	//glDrawArrays(GL_TRIANGLES,0,points);



	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);  // makes the 0th buffer "active"
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0); // associates 0th attribute with buffer
	glEnableVertexAttribArray(0);  // enable the 0th vertex attribute

	    vec3 color1;



	    color1 = vec3(1.0, 0.00, 0.0);
	glUniform3fv(glGetUniformLocation(renderingProgram, "vColor"), 1, value_ptr(color1));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	    glDrawElements(
	        GL_TRIANGLES,      // mode
	        ct->getNumTriangles()*3,    // count
	        GL_UNSIGNED_INT,   // type
	        0// (void*)0           // element array buffer offset
	    );
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	    color1 = glm::vec3(1.0, 1.0, 1.0);
	  	        glUniform3fv(glGetUniformLocation(renderingProgram, "u_color"), 1, value_ptr(color1));
	  	        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);

	  	        for(int i=0; i<ct->getNumTriangles() ; i++){
	  	            // cout<<"i*3:::"<<i*3<<endl;
	  	            glDrawElements(
	  	                GL_LINE_LOOP,      // mode
	  	                3,    // count
	  	                GL_UNSIGNED_INT,   // type
	  	                (void*)(i*3*sizeof(GL_UNSIGNED_INT))// (void*)0           // element array buffer offset
	  	            );
	  	        }


	  	    color1 = glm::vec3(0.2, 0.15, 0.4);
	  	        glUniform3fv(glGetUniformLocation(renderingProgram, "vColor"), 1, value_ptr(color1));

	  	        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
	  	        glDrawElements(
	  	            GL_TRIANGLES,      // mode
	  	            path_indices.size(),    // count
	  	            GL_UNSIGNED_INT,   // type
	  	            0// (void*)0           // element array buffer offset
	  	        );
	  	        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	  	        color1 = glm::vec3(1.0, 0.0, 0.0);
	  	            glUniform3fv(glGetUniformLocation(renderingProgram, "vColor"), 1, value_ptr(color1));
	  	            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);

	  	            for(int i=0; i<path_indices.size()/3 ; i++){
	  	                // cout<<"i*3:::"<<i*3<<endl;
	  	                glDrawElements(
	  	                    GL_LINE_LOOP,      // mode
	  	                    3,    // count
	  	                    GL_UNSIGNED_INT,   // type
	  	                    (void*)(i*3*sizeof(GL_UNSIGNED_INT))// (void*)0           // element array buffer offset
	  	                );
	  	            }






	//glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, 0);


	//glBindVertexArray(0);

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
