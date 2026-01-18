#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "Vector3.h"
#include "ray.h"
#include "aabb.h"

class material;

enum PRIMITIVETYPE{
    SPHERE,
    PLANE,
    TRIANGLE
};

struct hitDescriptor {
    Vector3 point;
    Vector3 normal;
    double trueroot;
    Color true_attenuation;
    Color emissive;
    material* hitMaterial;
    bool front_facing;
    double u,v;
    double brdf;
    double pdf;
};

class object {
    public:

    object() = default;
    virtual ~object() = default;

    virtual Vector3 getCenter(){
        return center;
    }

    virtual AABB getAABB() {
        return bound;
    }

    virtual material* getMat(){
        return mat;
    }

    virtual bool intersect(const ray& incident, hitDescriptor* hitstream){return false;}
    virtual void bindPrimitive() = 0;

    private:
    material* mat;
    AABB bound;
    Vector3 center;

};

void setFaceDirection(Vector3 &normal, Vector3 &direction, hitDescriptor* hitstream){
    hitstream->front_facing = (dot(normal, direction) < 0)?true:false;
}


#endif