#pragma once

#ifndef RAY_H
#define RAY_H

#include "Vector3.h"

class ray {
    public:

    explicit ray(){

    }
    
    ray(Vector3 orig, Vector3 dir):origin(orig), direction(dir){}

    Vector3 locate(double alpha) const{
        return origin + alpha*direction;
    }

    Vector3 get_origin() const{return origin;}

    Vector3 get_direction() const{return direction;}


    private:
    
    Vector3 origin;
    Vector3 direction;

};

#endif