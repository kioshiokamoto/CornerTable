#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "DefinitionTypes.h"


#include "CornerTable.h"

using namespace std;

void pointGenerate(double &x, double &y)
{
    double a1 = ((rand() % 1000001) / 1000000.0) / 5.0;
    double a2 = ((rand() % 1000001) / 1000000.0) / 5.0;
    double a3 = ((rand() % 1000001) / 1000000.0) / 5.0;
    double a4 = ((rand() % 1000001) / 1000000.0) / 5.0;
    double a5 = ((rand() % 1000001) / 1000000.0) / 5.0;
    double a6 = 1.0 - (a1 + a2 + a3 + a4 + a5);

    x = a1 * (2) + a2 * (1) + a3 * (-1) + a4 * (-2) + a5 * (-1) + a6 * (1);
    y = a1 * (0) + a2 * (1.73205) + a3 * (1.73205) + a4 * (0) + a5 * (-1.73205) + a6 * (-1.73205);    
}



int main()
{
	/*
    double x, y;
    srand(time(NULL));
    pointGenerate(x, y);
    
    printf("(%.8f, %.8f)\n", x, y);
    */

	ifstream fin;
	fin.open("mesh/mesh5.mesh");

	string name; //Off
	int npoint,ntriangle,var3;//Numero de puntos, orden y colores
	int var0;;
	fin>>name;
	cout<<name<<endl;
	fin>>npoint>>ntriangle>>var3;

	//llenamos la lista de puntos
	double* vertexList = new double[3*npoint];

	for(int i = 0; i<npoint;++i){
		fin>>vertexList[3*1]>>vertexList[3*i+1]>>vertexList[3*i+2];

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
	ct.printTriangleList();


    return 0;
}
