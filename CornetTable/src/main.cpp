//#define SPHERE // PARA PINTAR SPHERE
#define GRID // PARA PINTAR GRID

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
// #include <iostream>
// #include <fstream>
// #include <cmath>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Utils.hpp"
#include "CornerTable.h"
#include "min_path.h"

#define numVAOs 1
#define numVBOs 1 
#define numEBOs 2
#define normalize 2

using namespace std;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint ebo[numEBOs];

#ifdef GRID
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 0.8f; // grid cam D
#endif

#ifdef SPHERE
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 5.0f; // sphere cam 2D
#endif

float objLocX = 0.0f, objLocY = 0.0f, objLocZ = 0.0f;

int width, heigth;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

CornerTable *CT;
vector<unsigned int> path_indices;
int o, d;

void readMeshFiles() {
	#ifdef SPHERE
    string filePath = "offFiles/sphere.off"; 
	#endif
	#ifdef GRID
    string filePath = "offFiles/grid.off";
    #endif

    ifstream fin(filePath);
    cout<<"[LOG]\tFichero OFF cargado"<<endl;
    //First Line OFF
    string name;
    //Second Line
    int npoint, ntriangle, var3;
    // npoint +1 Line edges that form triangle
    int var0;
    fin>>name;
    fin>>npoint>>ntriangle>>var3;
    cout<<"[LOG]\tCantidades leidas"<<endl;

    /* Llenamos la lista de puntos */
    double* vertexList = new double[3*npoint];
    for (int i = 0; i < npoint; ++i)
        fin>>vertexList[3*i]>>vertexList[3*i+1]>>vertexList[3*i+2];
    cout<<"[LOG]\tVertices leidos"<<endl;

    const CornerType numberTriangles = ntriangle;
    const CornerType numberVertices = npoint;
    
    /* Fill the list with all the points */
    CornerType* triangleList = new CornerType[3*ntriangle];
    for (int i = 0; i <ntriangle; ++i)
        fin>>var0>>triangleList[i*3]>>triangleList[i*3+1]>>triangleList[i*3+2];
    cout<<"[LOG]\tIndices triangulos leidos"<<endl;
    
    int numberCoordinatesByVertex = 3;
    
    CornerTable *ct = new CornerTable(triangleList, vertexList,
        numberTriangles, numberVertices, numberCoordinatesByVertex);
    cout<<"[LOG]\tCorner Table creada"<<endl<<endl;
    
    /* Especificaciones */
    cout<<"\t\t -------------- OFF -------------- "<<endl;
    cout<<"\t\t (*) File: "<<filePath<<endl;
    cout<<"\t\t (*) Num vertices: "<<npoint<<endl;
    cout<<"\t\t (*) Num triangulos: "<<ntriangle<<endl;
    cout<<"\t\t (*) Num colores: "<<var3<<endl;
    cout<<"\t\t -------------- OFF -------------- "<<endl;

    CT = ct;
    fin.close();
}

void buildPath(int o, int d){
    vector<unsigned int> path;
    const CornerType *trianglesPositions = CT->getTriangleList();

    min_path(path, CT, o, d);
    
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

    // for(unsigned int i=0; i<path_indices.size(); i+=3){
    //     cout<<"I"<<" ::: "<<path_indices[i]<<","<<path_indices[i+1]<<","<<path_indices[i+2]<<endl;
    // }
}

void setupVertices(void) {

    double* vertexPositions = CT->getAttributes();
    const CornerType *trianglesPositions = CT->getTriangleList();
    
    for (unsigned int i=0; i<CT->getNumberVertices()*3; i++) // normalizar coordenadas
        vertexPositions[i] *= normalize;

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(numVBOs, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, CT->getNumberVertices()*3*sizeof(double), &vertexPositions[0], GL_STATIC_DRAW);

    glGenBuffers(numEBOs, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, CT->getNumTriangles()*3*sizeof(CornerType), &trianglesPositions[0], GL_STATIC_DRAW);
 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, path_indices.size()*3*sizeof(CornerType), &path_indices[0], GL_STATIC_DRAW);
}

void init(GLFWwindow *window) {
    renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
    glUseProgram(renderingProgram);
    
    buildPath(o, d);
    setupVertices();

    glfwGetFramebufferSize(window, &width, &heigth);
    aspect = (float)width / (float)heigth;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

}

void display(GLFWwindow *window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(renderingProgram);
	glUniformMatrix4fv(glGetUniformLocation(renderingProgram, "proj_matrix"), 1, GL_FALSE, glm::value_ptr(pMat));

    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ));
    mvMat = vMat * mMat;
	glUniformMatrix4fv(glGetUniformLocation(renderingProgram, "mv_matrix"), 1, GL_FALSE, glm::value_ptr(mvMat));

    // ------------------ VBO VERTICES - Bind vertices ------------------
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);  // makes the 0th buffer "active"
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0); // associates 0th attribute with buffer
    glEnableVertexAttribArray(0);  // enable the 0th vertex attribute

    glm::vec3 color1;

    // ------------------ EBO ALL Triangles - Bind EBO and PAINT MESH ------------------
    color1 = glm::vec3(0.7, 0.75, 0.4);
    glUniform3fv(glGetUniformLocation(renderingProgram, "u_color"), 1, value_ptr(color1));


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glDrawElements(
        GL_TRIANGLES,      // mode
        CT->getNumTriangles()*3,    // count
        GL_UNSIGNED_INT,   // type
        0// (void*)0           // element array buffer offset
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // ------------------ PAINT LINES (ALL TRIANGLES BORDER) ------------------
    color1 = glm::vec3(1.0, 1.0, 1.0);
    glUniform3fv(glGetUniformLocation(renderingProgram, "u_color"), 1, value_ptr(color1));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);

    for(int i=0; i<CT->getNumTriangles() ; i++){
        // cout<<"i*3:::"<<i*3<<endl;
        glDrawElements(
            GL_LINE_LOOP,      // mode
            3,    // count
            GL_UNSIGNED_INT,   // type
            (void*)(i*3*sizeof(GL_UNSIGNED_INT))// (void*)0           // element array buffer offset
        );
    }

    // ------------------ EBO PATH Triangles - Bind EBO and PAINT PATH ------------------
    color1 = glm::vec3(0.2, 0.15, 0.4);
    glUniform3fv(glGetUniformLocation(renderingProgram, "u_color"), 1, value_ptr(color1));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
    glDrawElements(
        GL_TRIANGLES,      // mode
        path_indices.size(),    // count
        GL_UNSIGNED_INT,   // type
        0// (void*)0           // element array buffer offset
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // ------------------ PAINT LINES (PATH TRIANGLES BORDER) ------------------
    color1 = glm::vec3(1.0, 0.0, 0.0);
    glUniform3fv(glGetUniformLocation(renderingProgram, "u_color"), 1, value_ptr(color1));
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

    // ------------------ PAINT POINTS (TRIANGLES VERTICES) ------------------ 
    color1 = glm::vec3(0.0, 0.0, 0.0);
    glUniform3fv(glGetUniformLocation(renderingProgram, "u_color"), 1, value_ptr(color1));
    glPointSize(4.0); 
    glDrawArrays(GL_POINTS, 0, CT->getNumberVertices());

}

int main() {
    system("cls");
    readMeshFiles();
    cout<<endl;
    // cout<<"\t ========================================== "<<endl;
    cout<<"\t\t..:: CAMINO MINIMO ENTRE 2 TRIANGULOS ::.."<<endl;
    // cout<<"\t ========================================== "<<endl;
    cout<<endl;
    cout<<"\t(*) Ingrese el triangulo de origen (0-"<<CT->getNumTriangles()-1<<"): ";
    cin>>o;
    cout<<"\t(*) Ingrese el triangulo de destino (0-"<<CT->getNumTriangles()-1<<"): ";
    cin>>d;

    if (!glfwInit()) exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    GLFWwindow * window = glfwCreateWindow(800, 800, "Ch 4 - program 1", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) exit(EXIT_FAILURE);
    cout<<"GL_VERSION: "<<glGetString(GL_VERSION)<<endl;
    glfwSwapInterval(1);

    init(window);

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, vbo);
    glDeleteBuffers(1, ebo);
    glDeleteShader(renderingProgram);
    glDeleteVertexArrays(1, vao);
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
/*
g++ -o2 -Wall main.cpp -o main -L..\..\dependencies -lglew32s -lglu32 -lopengl32 -lglfw3 -mwindows -mconsole
*/
