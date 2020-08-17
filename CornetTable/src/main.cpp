// Incluir librerias necesarias
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <string>
#include <fstream>


// Incluir GLEW
#define GLEW_STATIC
#include <GL/glew.h>


// Incluir GLFW
#include <GLFW/glfw3.h>


// Incluir recursos creados
#include "Utils.h"
#include "CornerTable.h"
#include "DefinitionTypes.h"
#include "camino_minimo.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
using namespace std;


#define numVAOs 1
#define numVBOs 1
#define numEBOs 2
#define normalize 2

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint ebo[numEBOs];

//Variables globales
int cantidadPuntos;
double* vertexList1;
int points;
CornerTable *ct;
vector<unsigned int> indiceCamino;

void generadorPuntos(double &x1, double &y1,  double &x2 ,double &y2){
    srand( time( NULL ) );

    x1 = -3.0000 + (rand( ) % 60010 ) / 10000.0f;
    y1= -3.0000 + (rand( ) % 60010 ) / 10000.0f;
    x2 = -3.0000 + (rand( ) % 60010 ) / 10000.0f;
    y2= -3.0000 + (rand( ) % 60010 ) / 10000.0f;

}

void ubicarCoordenada(double vertexList[],int npoint,double punto1x, double punto1y,int &punto){
	double tempx,tempy;
	double cercanox,cercanoy;
	double cercanoxComparador,cercanoyComparador;
	cercanoxComparador = 1000;
	cercanoyComparador = 1000;

	for(int i = 1 ; i <= npoint ;i++){
		tempx = vertexList[(i-1)*3];
		tempy = vertexList[(i-1)*3+1];
		cercanox = tempx - punto1x;
		if(cercanox<=0){
			cercanox = cercanox*(-1.0);
		}
		cercanoy = tempy - punto1y;
		if(cercanoy<=0){
			cercanoy = cercanoy*(-1.0);
		}
		if(((punto1x - tempx) == 0) && ((punto1y - tempy) == 0)){
			punto = i-1;
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


void leerMesh(){
	ifstream fin;
	fin.open("mesh/mesh21.mesh");
	string name;
	int npoint,ntriangle,var3;//Numero de puntos, orden y colores
	int var0;;
	fin>>name;
	fin>>npoint>>ntriangle>>var3;


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
									numberCoordinatesByVertex
	);
	ct = CT;
	cantidadPuntos = npoint;
	vertexList1= vertexList;

}

void crearCamino(int inicio, int destino){
    vector<unsigned int> camino;
    const CornerType *trianglesPositions = ct->getTriangleList();

    min_path(camino, ct, inicio, destino);

    int aux = camino[destino];
    indiceCamino.push_back(trianglesPositions[destino*3]);
    indiceCamino.push_back(trianglesPositions[destino*3 + 1]);
    indiceCamino.push_back(trianglesPositions[destino*3 + 2]);

    while(aux>-1){
    	indiceCamino.push_back(trianglesPositions[aux*3]);
    	indiceCamino.push_back(trianglesPositions[aux*3 + 1]);
    	indiceCamino.push_back(trianglesPositions[aux*3 + 2]);
        aux = camino[aux];
    }

}
void buffers() {

    double* vertexPositions = ct->getAttributes();
    const CornerType *trianglesPositions = ct->getTriangleList();

    for (unsigned int i=0; i<ct->getNumberVertices()*3; i++)
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiceCamino.size()*3*sizeof(CornerType), &indiceCamino[0], GL_STATIC_DRAW);
}

void init (GLFWwindow* window) {

	// Shaders
	renderingProgram = Utils::createShaderProgram("src/vertShader.glsl", "src/fragShader.glsl");

	// Lectura de mesh
	leerMesh();

	// Generacion de puntos
	double punto1x,punto1y,punto2x,punto2y;
	generadorPuntos(punto1x,punto1y,punto2x,punto2y);
	int puntoCercano1,puntoCercano2;

	//	Ubicacion de puntos
	ubicarCoordenada(vertexList1,cantidadPuntos,punto1x,punto1y,puntoCercano1);
	ubicarCoordenada(vertexList1,cantidadPuntos,punto2x,punto2y,puntoCercano2);
	cout<<"[" << punto1x <<" - " <<punto1y<< "]";
	cout<<" - Es cercano a el punto "<<puntoCercano1<<endl;
	cout<<"[" << punto2x <<" - " <<punto2y<< "]";
	cout<<" - Es cercano a el punto "<<puntoCercano2<<endl;

	//	Distancia minima de puntos
	crearCamino(puntoCercano1,puntoCercano2);

	//	Mostrar ID's de triangulos recorridos
	for(unsigned int i=0; i<indiceCamino.size(); i+=3){
	    cout<<i/3+1<<" : "<<indiceCamino[i]<<" , "<<indiceCamino[i+1]<<" , "<<indiceCamino[i+2]<<endl;
	 }

	// Declarar buffers
	buffers();

}

void display(GLFWwindow* window, double currentTime) {

	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	vec3 color1;
	color1 = vec3(1.0, 0.00, 0.0);
	glUniform3fv(glGetUniformLocation(renderingProgram, "vColor"), 1, value_ptr(color1));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	//Dibujo de CORNER TABLE
	for(int i=0; i<ct->getNumTriangles() ; i++){
		color1 = vec3(i%3-0.42, i%5-0.42, i%7-0.43);
		if(i==0){
			color1 = vec3(1, 0.42, 0.23);
		}
			glUniform3fv(glGetUniformLocation(renderingProgram, "vColor"), 1, value_ptr(color1));
	            glDrawElements(
	                GL_TRIANGLES,
	                3,
	                GL_UNSIGNED_INT,
	                (void*)(i*3*sizeof(GL_UNSIGNED_INT))
	            );
	        }

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//DIBUJO DE RECORRIDO DE TRIANGULOS
    color1 = vec3(1, 1, 1);
    glUniform3fv(glGetUniformLocation(renderingProgram, "vColor"), 1, value_ptr(color1));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
	glDrawElements(
	        GL_TRIANGLES,
			indiceCamino.size(),
	        GL_UNSIGNED_INT,
	        0
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// TRAZADO DE TRIANGULOS
	color1 = glm::vec3(0.0, 0.0, 0.0);
	glUniform3fv(glGetUniformLocation(renderingProgram, "vColor"), 1, value_ptr(color1));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
    for(int i=0; i<indiceCamino.size()/3 ; i++){
    	glDrawElements(
                 GL_LINE_LOOP,
                 3,
                 GL_UNSIGNED_INT,
                 (void*)(i*3*sizeof(GL_UNSIGNED_INT))
 	     );
	 }

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
