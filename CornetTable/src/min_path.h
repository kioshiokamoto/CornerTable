#include "DefinitionTypes.h"
#include "CornerTable.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <set>

using namespace std;

struct Triangle{
    unsigned int vertex1, vertex2, vertex3;
};


#define INF 9999

int minCost(vector<unsigned int> cost, vector<bool> marked){
    int min = INF;
    int i_min = -1;
    for (unsigned int i=0; i<cost.size(); i++){
        if (!marked[i] && cost[i]<min){
            min = cost[i];
            i_min = i;
        }
    }
    return i_min;
}

void DijkstraPath(vector<unsigned int> &path, vector<vector<unsigned int>> adjmat, int i, int d){

    vector<bool> marked(adjmat.size(), 0);
    vector<unsigned int> cost(adjmat.size(), INF);
    path.resize(adjmat.size());
    fill(path.begin(), path.end(), -1);

    cost[i] = 0;

    for (unsigned int i=0; i<adjmat.size(); i++){
        int i_node = minCost(cost, marked);

        if (i_node == -1) // all is marked
            break;

        marked[i_node] = true;

        for (unsigned int j=0; j<adjmat.size(); j++){
            if (marked[j] || !adjmat[i_node][j]) continue;
            if (cost[i_node] + adjmat[i_node][j] < cost[j]){
                cost[j] =  cost[i_node] + adjmat[i_node][j];
                path[j] = i_node;
            }
        }
    }
}

bool isConected(Triangle A, Triangle B){

    set<unsigned int> sa;
    sa.insert(A.vertex1);
    sa.insert(A.vertex2);
    sa.insert(A.vertex3);

    int cont = 0;
    if (sa.find(B.vertex1) != sa.end()) cont++;
    if (sa.find(B.vertex2) != sa.end()) cont++;
    if (sa.find(B.vertex3) != sa.end()) cont++;

    return cont>1;
}



void buildAdjMat(CornerTable* CT, vector<vector<unsigned int>> &adjMat){
    const CornerType* triangleList = CT->getTriangleList();
    unsigned int numTriangles = CT->getNumTriangles();
    adjMat.resize(numTriangles);
    fill(adjMat.begin(), adjMat.end(), vector<unsigned int>(numTriangles, 0));

    for (unsigned int i=0; i<numTriangles; i++){
        Triangle v{triangleList[3*i],
                triangleList[3*i + 1],
                triangleList[3*i + 2]
                };
        for (unsigned int j=i+1; j<numTriangles; j++){
            Triangle comp{triangleList[3*j],
                        triangleList[3*j + 1],
                        triangleList[3*j + 2]
                        };
            if (isConected(v, comp)){
                adjMat[i][j] = 1;
                adjMat[j][i] = 1;
            }
        }
    }
}

void PrintDijkstraPath(vector<unsigned int> path, int o, int d){
    cout<<"Camino: "<<endl;
    int r = path[d];
    cout<<d<<" <- ";
    while(r > -1 && r != o){
        cout<<r<<" <- ";
        r = path[r];
    }
    cout<<o<<endl;
}

void min_path(vector<unsigned int> &path, CornerTable *CT, int o, int d) {

    vector<vector<unsigned int>> adjmat; buildAdjMat(CT, adjmat);

    if (CT->getNumTriangles() < 20){ // imprime la matriz de adyacencia solo si no es tan grande
        cout<<"\nMatriz de adyacencia: "<<endl;
        for(unsigned int i =0; i<adjmat.size(); i++){
            for (unsigned int j=0; j<adjmat.size(); j++){
                cout<<adjmat[i][j]<<" ";
            }
            cout<<endl;
        }
    }

    DijkstraPath(path, adjmat, o, d);

}

