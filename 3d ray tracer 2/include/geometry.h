#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>
#include <iostream>

struct vector2 {
    double x;
    double y;

    vector2(double x = 0, double y = 0) : x(x), y(y) {}

    vector2 operator+(const vector2& other) const {
        return vector2(x + other.x, y + other.y);
    }
    
    vector2 operator-(const vector2& other) const {
        return vector2(x - other.x, y - other.y);
    }
    
    vector2 operator*(const double& other) const {
        return vector2(x * other, y * other);
    }
    
    vector2 operator/(const double& other) const {
        return vector2(x / other, y / other);
    }
    
    double magnitude() const {
        return sqrt(x * x + y * y);
    }

    void print(){
        std::cout << x << ", " << y << std::endl;
    }
};

struct vector3 {
    double x;
    double y;
    double z;

    vector3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    vector3 operator+(vector3 other) const {
        return vector3(x + other.x, y + other.y, z + other.z);
    }

    vector3 operator-(vector3 other) const {
        return vector3(x - other.x, y - other.y, z - other.z);
    }

    vector3 operator*(vector3 other) const {
        return vector3(x * other.x, y * other.y, z * other.z);
    }

    vector3 operator*(double other) const {
        return vector3(x * other, y * other, z * other);
    }

    vector3 operator/(double other) const {
        return vector3(x / other, y / other, z / other);
    }

    double magnitude() const {
        return sqrt(x * x + y * y + z * z);
    }

    vector3& normalize() {
        float mag = this->magnitude();
        if (mag != 0) {
            x /= mag;
            y /= mag;
            z /= mag;
        }
        return *this;
    }

    void print(){
        std::cout << x << ", " << y << ", " << z << std::endl;
    }
};

vector3 operator+(double scalar, const vector3& vec) {
    return vector3(scalar + vec.x, scalar + vec.y, scalar + vec.z);
}

vector3 operator-(double scalar, const vector3& vec) {
    return vector3(scalar - vec.x, scalar - vec.y, scalar - vec.z);
}

vector3 operator*(double scalar, const vector3& vec) {
    return vector3(scalar * vec.x, scalar * vec.y, scalar * vec.z);
}

vector3 operator/(double scalar, const vector3& vec) {
    return vector3(scalar / vec.x, scalar / vec.y, scalar / vec.z);
}

double distance_2d(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

double distance_2d_v(vector2 p1, vector2 p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

#endif