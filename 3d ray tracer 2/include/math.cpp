#include <vector>
#include "geometry.h"

using namespace std;

typedef vector<vector<double>> Matrix;

vector3 cross_multiply(vector3 a, vector3 b){
    return vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

double dot_product(vector3 a, vector3 b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

vector3 matrix_multiply(vector3 vector, Matrix matrix){
    vector3 new_vector(0, 0, 0);

    new_vector.x = matrix[0][0] * vector.x + matrix[0][1] * vector.y + matrix[0][2] * vector.z;
    new_vector.y = matrix[1][0] * vector.x + matrix[1][1] * vector.y + matrix[1][2] * vector.z;
    new_vector.z = matrix[2][0] * vector.x + matrix[2][1] * vector.y + matrix[2][2] * vector.z;

    return new_vector;
}

float cube(float x) {
    return x * x * x;
}

float square(float x) {
    return x * x;
}

// useless function
float power9(float x){
    return x * x * x * x * x * x * x * x * x;
}
