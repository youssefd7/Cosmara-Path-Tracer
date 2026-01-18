#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"
#include "material.h"
#include "utils.h"
#include <cmath>

class sphere : public object {
    public:

    sphere(double _radius, Vector3 _center, material* _mat):
    radius(_radius), center(_center), mat(_mat){
        this->bindPrimitive();
    }

    Vector3 getCenter() override {
        return center;
    }

    AABB getAABB() override {
        return boundbox;
    }

    bool intersect(const ray& incident, hitDescriptor* hitstream) override {
        Vector3 rayDirection = unit(incident.get_direction());
        Vector3 origin_center = incident.get_origin()-center;
        bool intersects = false;
        double a = 1;
        double b = 2 * (dot(rayDirection, origin_center));
        double c = dot(origin_center, origin_center) - (radius*radius);
        double discriminant = (b*b) - 4*(c);
        intersects = (discriminant >= 0) ? true:false;
        if (!intersects){return intersects;}
        double sqrtDiscriminant = sqrt(discriminant);
        double trueroot;
        double t1 = (-b-sqrtDiscriminant)/(2*a);
        if (t1 > EPSILON){
            trueroot = t1;
        }
        else {
            double t2 = (-b+sqrtDiscriminant)/(2*a);
            if (t2 < EPSILON){
                return false;
            }
            trueroot = t2;
        }

        Vector3 point = incident.locate(trueroot);
        Vector3 normal = unit(point-center);
        hitstream->point = point;
        hitstream->normal = normal;
        hitstream->trueroot = trueroot;
        hitstream->hitMaterial = mat;
        setFaceDirection(normal, rayDirection, hitstream);
        Vector3 P = -normal;
        hitstream->u = (atan2(P.z(), -P.x())/(2*PI) + 0.5);
        hitstream->v =  0.5+asin(P.y())/PI;
        return intersects;
    }

    void bindPrimitive() override {
        Vector3 radiusvec = Vector3(radius, radius, radius);
        AABB boundingBox = AABB((center - radiusvec), (center + radiusvec));
        boundbox = boundingBox;
    }

    material* getMat() override {
        return mat;
    }


    private:
    double radius = 1;
    Vector3 center;
    material* mat;
    AABB boundbox;

};

#endif