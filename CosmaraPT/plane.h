#pragma once

#ifndef PLANE_H
#define PLANE_H

#include "object.h"
#include "material.h"
#include "utils.h"
#include <cmath>


class plane : public object {
    public:

    plane(Vector3 _knownp, Vector3 _u, Vector3 _v, material* _mat):
    knownp(_knownp), u(_u), v(_v),mat(_mat){
        this->bindPrimitive();
        center = knownp + 0.5*u + 0.5*v;
        normal = unit(cross(u,v));
        aLength = u.magnitude();
        bLength = v.magnitude();
    }

    Vector3 getCenter() override {
        return center;
    }

    AABB getAABB() override {
        return boundbox;
    }

    bool intersect(const ray& incident, hitDescriptor* hitstream) override {
        bool intersects = false;
        Vector3 rayDirection = incident.get_direction();
        double num = dot((knownp-incident.get_origin()), normal);
        double denom = dot(normal, rayDirection);
        if (denom == 0) {
            return false;
        }
        double trueroot = num/denom;
        if (trueroot <= 0){return false;}
        Vector3 point = incident.locate(trueroot);
        Vector3 pointRelativeToKnownP = point - knownp;
        
        double alphaComponent = dot(pointRelativeToKnownP, u);
        double betaComponent = dot(pointRelativeToKnownP, v);

        if (alphaComponent >= 0 && alphaComponent <= aLength*aLength && betaComponent >= 0 && betaComponent <= bLength*bLength) {
            intersects = true;
            hitstream->point = point;
            hitstream->normal = normal;
            hitstream->trueroot = trueroot;
            hitstream->hitMaterial = mat;
            setFaceDirection(normal, rayDirection, hitstream);
        }
        return intersects;
    }

    void bindPrimitive() override{
        Vector3 widthVec = unit(cross(u,v));
        AABB boundingBox = AABB(
        Vector3(std::min({knownp.x(), knownp.x()+u.x(), knownp.x()+v.x(), knownp.x()+u.x()+v.x()}),
        std::min({knownp.y(), knownp.y()+u.y(), knownp.y()+v.y(), knownp.y()+u.y()+v.y()}),
        std::min({knownp.z(), knownp.z()+u.z(), knownp.z()+v.z(), knownp.z()+u.z()+v.z()})),

        Vector3(std::max({knownp.x(), knownp.x()+u.x(), knownp.x()+v.x(), knownp.x()+u.x()+v.x()}),
        std::max({knownp.y(), knownp.y()+u.y(), knownp.y()+v.y(), knownp.y()+u.y()+v.y()}),
        std::max({knownp.z(), knownp.z()+u.z(), knownp.z()+v.z(), knownp.z()+u.z()+v.z()})));
        boundbox = boundingBox;
    }

    material* getMat() override {
        return mat;
    }

    private:
    Vector3 knownp;
    Vector3 u;
    Vector3 v;
    material* mat;
    AABB boundbox;
    Vector3 center;
    Vector3 normal;
    double aLength;
    double bLength;
};


#endif