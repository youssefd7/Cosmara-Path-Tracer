#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "object.h"
#include "material.h"
#include "utils.h"
#include "triangle.h"
#include "bvh.h"
#include <cmath>

class model : public object {
    public:

    model(std::vector<object*> _comps, material* _mat):triangles(_comps), mat(_mat){
        this->bindPrimitive();
        center = (boundbox.pointmin + boundbox.pointmax)/2.0f;
        BVHNode* modelnode = new BVHNode;
        modelnode = modelnode->buildBVH(triangles, 0, triangles.size(), 0);
        this->modelNode = modelnode;
    };

    Vector3 getCenter() override {
        return center;
    }

    AABB getAABB() override {
        return boundbox;
    }

    bool intersect(const ray& incident, hitDescriptor* hitstream) override {
        return this->modelNode->intersect(incident, hitstream, 0, triangles);
    }

    void bindPrimitive() override {
        boundbox = computeBoundingVolume(triangles);
    }

    material* getMat() override {
        return mat;
    }

    static AABB computeBoundingVolume(std::vector<object*> tris){
        AABB modelbox = tris[0]->getAABB();
        for (unsigned long i = 1; i < tris.size(); ++i){
            AABB currentBox = tris[i]->getAABB();
            modelbox = BVHNode::makeBox(modelbox, currentBox);
        }
        return modelbox;
    }

    private:
    std::vector<object*> triangles;
    BVHNode* modelNode;
    Vector3 center;
    AABB boundbox;
    material* mat;
    ;
};


#endif