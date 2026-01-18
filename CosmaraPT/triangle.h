#pragma once
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object.h"
#include "material.h"
#include "utils.h"
#include <cmath>

class triangle : public object {
    public:

    triangle(Vector3 _v1, Vector3 _v2, Vector3 _v3, material* _mat):v1(_v1), v2(_v2), v3(_v3), mat(_mat){
        center = Vector3((v1.x()+v2.x()+v3.x())/3.0f, (v1.y()+v2.y()+v3.y())/3.0f, (v1.z()+v2.z()+v3.z())/3.0f);
        bindPrimitive();
    }

    Vector3 getCenter() override {
        return center;
    }

    AABB getAABB() override {
        return boundbox;
    }

    bool intersect(const ray& incident, hitDescriptor* hitstream) override {
        Vector3 edge1 = v2-v1;
        Vector3 edge2 = v3-v1;
        Vector3 direction = incident.get_direction();
        Vector3 raycross = cross(direction, edge2);
        double det = dot(edge1, raycross);
        if (det > -EPSILON && det < EPSILON){
            return false;
        }
        float inv_det = 1.0f/det;
        Vector3 dir = incident.get_origin()-v1;
        double u = inv_det * dot(raycross,dir);
        if (u < 0 || u > 1){
            return false;
        }
        Vector3 dircross = cross(dir, edge1);
        double v = inv_det * dot(direction, dircross);
        if (v < 0 || u+v > 1){
            return false;
        }
        double trueroot = inv_det * dot(edge2, dircross);
        if (trueroot > EPSILON){
            hitstream->trueroot = trueroot;
            hitstream->normal = unit(cross(edge1, edge2));
            hitstream->point = incident.locate(trueroot);
            hitstream->hitMaterial = mat;
            setFaceDirection(hitstream->normal, direction, hitstream);
            return true;
        }
        return false;
    }

    void bindPrimitive() override{
        AABB boundingBox = AABB(
            Vector3(std::min({v1.x(), v2.x(), v3.x()}), std::min({v1.y(), v2.y(), v3.y()}), std::min({v1.z(), v2.z(), v3.z()})),
            Vector3(std::max({v1.x(), v2.x(), v3.x()}), std::max({v1.y(), v2.y(), v3.y()}), std::max({v1.z(), v2.z(), v3.z()}))
        );
        boundbox = boundingBox;
    }

    material* getMat() override {
        return mat;
    }

    private:
    Vector3 v1;
    Vector3 v2;
    Vector3 v3;

    Vector3 center;
    AABB boundbox;
    material* mat;
    ;
};

#endif