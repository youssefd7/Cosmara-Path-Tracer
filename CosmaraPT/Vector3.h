#pragma once

#ifndef VECTOR3
#define VECTOR3

#include "utils.h"
#include <math.h>
#include <iostream>

class Vector3{
    public:

    explicit Vector3(){
        comps[0] = 0;
        comps[1] = 0;
        comps[2] = 0;
    }
    
    Vector3(double const x, double const y, double const z){
        comps[0] = x;
        comps[1] = y;
        comps[2] = z;    
    }

    double x() const{return comps[0];}
    double y() const{return comps[1];}
    double z() const{return comps[2];}

    void operator+=(Vector3 const& addend){
        comps[0] += addend.x();
        comps[1] += addend.y();
        comps[2] += addend.z();
    }

    void operator-=(Vector3 const& subtrahend){
        comps[0] -= subtrahend.x();
        comps[1] -= subtrahend.y();
        comps[2] -= subtrahend.z();
    }

    void operator*=(Vector3 const& multiplier){
        comps[0] *= multiplier.x();
        comps[1] *= multiplier.y();
        comps[2] *= multiplier.z();
    }

    void operator/=(Vector3 const& divisor){
        comps[0] *= divisor.x();
        comps[1] *= divisor.y();
        comps[2] *= divisor.z();
    }

    Vector3 operator-(){
        return Vector3(-comps[0], -comps[1], -comps[2]);
    }


    bool operator==(Vector3 const& v1){
        return (comps[0] == v1.x() && comps[1] == v1.y() && comps[2] == v1.z());
    }

    double magnitude() const{
        return sqrt(this->squared_sum());
    }

    double squared_sum() const{
        return (comps[0]*comps[0] + comps[1]*comps[1] + comps[2]*comps[2]);
    }

    static Vector3 random(){
        return Vector3(random_dbl_range(-1,1), random_dbl_range(-1,1), random_dbl_range(-1,1));
    }

    private:
    double comps[3];
};

Vector3 operator+(Vector3 const& v1, Vector3 const& v2){
    return Vector3(v1.x()+v2.x(), v1.y()+v2.y(), v1.z()+v2.z());
}

Vector3 operator-(Vector3 const& v1, Vector3 const& v2){
    return Vector3(v1.x()-v2.x(), v1.y()-v2.y(), v1.z()-v2.z());
}

Vector3 operator*(Vector3 const& v1, Vector3 const& v2){
    return Vector3(v1.x()*v2.x(), v1.y()*v2.y(), v1.z()*v2.z());
}

Vector3 operator/(Vector3 const& v1, Vector3 const& v2){
    return Vector3(v1.x()/v2.x(), v1.y()/v2.y(), v1.z()/v2.z());
}

Vector3 operator*(double scalar, Vector3 const& v1){
    return Vector3(scalar*v1.x(), scalar*v1.y(), scalar*v1.z());
}

Vector3 operator*(Vector3 const& v1, double scalar){
    return Vector3(scalar*v1.x(), scalar*v1.y(), scalar*v1.z());
}

Vector3 operator/(double scalar, Vector3 const& v1){
    return v1*(1/scalar);
}

Vector3 operator/(Vector3 const& v1, double scalar){
    return v1*(1/scalar);
}

inline Vector3 unit(Vector3 const& v1){
    return v1 * (1/v1.magnitude());
}

inline double dot(Vector3 const& v1, Vector3 const& v2){
    return v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z();
}

inline Vector3 cross(Vector3 const& v1, Vector3 const& v2){
    return Vector3(
        v1.y()*v2.z() - v1.z()*v2.y(),
        v1.z()*v2.x() - v1.x()*v2.z(),
        v1.x()*v2.y() - v1.y()*v2.x()
    );
}

Vector3 generateRandomPointUnitSphere(){
    double azimuthal = 2*PI*random_dbl();
    double polar = 2*PI*random_dbl();

    Vector3 direction(cos(azimuthal)*cos(polar), sin(azimuthal)*cos(polar), sin(polar));
    
    return direction;
}

Vector3 generateDirInHemisphere(Vector3& normal, DISTRIBUTION dist){
    if (dist == UNIFORM){
        Vector3 randomDir = generateRandomPointUnitSphere();
        if (dot(randomDir, normal) < 0){return -randomDir;}
        return randomDir;
    }
    else if (dist == COSINE)
    {
        Vector3 randomDir = generateRandomPointUnitSphere();
        Vector3 finalDir = normal + randomDir;
        finalDir = nearZero(finalDir.magnitude()) ? normal : unit(finalDir);
        return finalDir;
    }
    return Vector3(0,0,0);
};

inline Vector3 reflect(const Vector3& direction, const Vector3& normal){
    return direction - 2*normal*dot(normal, direction);
}

std::ostream& operator << (std::ostream& stream, Vector3 const& v1){
    stream << v1.x() << " " << v1.y() << " " << v1.z() << (char)10;
    return stream;
};

bool operator<(Vector3 const& v1, Vector3 const& v2){
    return (v1.x() < v2.x() && v1.y() < v2.y() && v1.z() < v2.z()) ? true:false;
}

bool operator>(Vector3 const& v1, Vector3 const& v2){
    return (v1.x() > v2.x() && v1.y() > v2.y() && v1.z() > v2.z()) ? true:false;
}

bool operator<=(Vector3 const& v1, Vector3 const& v2){
    return (v1.x() <= v2.x() && v1.y() <= v2.y() && v1.z() <= v2.z()) ? true:false;
}

bool operator>=(Vector3 const& v1, Vector3 const& v2){
    return (v1.x() >= v2.x() && v1.y() >= v2.y() && v1.z() >= v2.z()) ? true:false;
}


#endif