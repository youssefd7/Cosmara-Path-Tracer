#pragma once

#ifndef AABB_H
#define AABB_H

#include "utils.h"
#include "ray.h"
#include <cmath>
#include <algorithm>


class AABB{
    public:
    Vector3 pointmin;
    Vector3 pointmax;
    AABB() = default;
    AABB(Vector3 _pointmin, Vector3 _pointmax):
    pointmin(_pointmin),pointmax(_pointmax){}

    bool intersect(const ray &incident) const{
        float tmin = 0.0, tmax = infinity;
        const float epsilon = 1e-6;
        Vector3 rayorig = incident.get_origin();
        Vector3 raydir = incident.get_direction();

        float t1 = (pointmin.x() - rayorig.x())/raydir.x();
        float t2 = (pointmax.x() - rayorig.x())/raydir.x();
        float t3 = (pointmin.y() - rayorig.y())/raydir.y();
        float t4 = (pointmax.y() - rayorig.y())/raydir.y();
        float t5 = (pointmin.z() - rayorig.z())/raydir.z();
        float t6 = (pointmax.z() - rayorig.z())/raydir.z();

        tmin = std::max({tmin,std::min(t1,t2), std::min(t3,t4), std::min(t5,t6)}) - epsilon;
        tmax = std::min({tmax,std::max(t1,t2), std::max(t3,t4), std::max(t5,t6)}) + epsilon;
        return tmin <= tmax && tmax >= 0;
    }

    float surfaceArea(){
        float width = abs(pointmax.x()-pointmin.x());
        float height = abs(pointmax.y()-pointmin.y());
        float depth = abs(pointmax.z()-pointmin.z());
        return 2.0f*(width*height + height*depth + depth*width);
    }

    int findLongestAxis(){
        if (abs(pointmax.x()-pointmin.x()) > abs(pointmax.y()-pointmin.y())){
            return abs(pointmax.x()-pointmin.x()) > abs(pointmax.z()-pointmin.z()) ? 0:2;
        }
        else {
            return abs(pointmax.y()-pointmin.y()) > abs(pointmax.z()-pointmin.z()) ? 1:2;
        }
    }

    private:
};

#endif