#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <random>
const double PI = 3.141592653589793238462643383279;
const double EPSILON = 1e-3;
const double infinity = std::numeric_limits<double>::max();

enum DISTRIBUTION {
    UNIFORM,
    COSINE,
    BECKMANN,
};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0,1);

inline double rad(double deg){
    return deg*(PI/180);
}

inline double deg(double rad){
    return rad*(180/PI);
}

inline double min(double n1, double n2){
    double maxnum = n1;
    maxnum = (n1 < n2) ? n1:n2;
    return maxnum;
}

inline double clamp(double i1, double p1, double p2){
    if (p1 <= i1 && p2 >= i1){return i1;}
    return (i1>p2)?p2:p1;
}

inline double random_dbl(){
    return dis(gen);
}

inline double random_dbl_range(double r1, double r2){
    double randratio = random_dbl();
    return r1+randratio*(r2-r1);
}

inline bool nearZero(double param){
    if (param < EPSILON && param > -EPSILON){
        return true;
    }
    return false;
}

template <typename T>
void print(std::vector<T> vec){
    for (unsigned long i = 0; i < vec.size()-1; ++i){
        std::cout << vec[i] << " ";
    }
    std::cout << vec[vec.size()-1] << (char)10;
}

#endif