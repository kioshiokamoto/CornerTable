#include <cstdio>
#include <ctime>
#include <cstdlib>
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
    double x, y;
    srand(time(NULL));
    pointGenerate(x, y);
    
    printf("(%.8f, %.8f)\n", x, y);
    return 0;
}
