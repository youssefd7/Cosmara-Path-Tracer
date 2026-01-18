#pragma once

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "object.h"
#include "utils.h"
#include "material.h"
#include "bvh.h"


class scene {
    public:
    scene() = default;
    scene(std::vector<object*> objectArray):world(objectArray){
        BVH = new BVHNode;
        BVH = BVH->buildBVH(objectArray, 0, objectArray.size(), 0);
    }

    void initBVH(){
        BVH = new BVHNode;
        BVH = BVH->buildBVH(world, 0, world.size(), 0);
    }

    ~scene(){
        for (unsigned long i = 0; i < world.size(); i++){
            delete world[i];
        }
    }
    void clear(){
        this->world.clear();
    }
    
    void addObject(object* objectptr){
        if (objectptr){world.emplace_back(objectptr);}
    }

    void extendWorld(std::vector<object*>& model){
        for (unsigned long i = 0; i < model.size(); ++i){
            world.emplace_back(model[i]);
        }
    }

    bool checkClosestRoot(double* closestroot, double prospectroot) const{
        if (prospectroot < *closestroot){
            *closestroot = prospectroot;
            return true;
        }
        return false;
    }

    bool intersectWorld(const ray& incident, hitDescriptor* hitIntel) {
        bool intersected = false;
        double closestroot = infinity;
        hitDescriptor closestHit;
        for (unsigned long i = 0; i < world.size(); ++i){
            if (world[i]->intersect(incident, hitIntel)){
                if (!intersected){intersected = true;}
                if (checkClosestRoot(&closestroot, hitIntel->trueroot)){
                    closestHit = *hitIntel;
                }
            }
        }
        *hitIntel = closestHit;
        return intersected;
    }

    void reserveObjectCapacity(unsigned long long count){
        world.reserve(world.size()+count);
    }

    std::vector<object*> retrieveWorld(){
        return world;
    }

    bool intersectWorldBVH(const ray& incident, hitDescriptor* hitIntel){
        return BVH->intersect(incident, hitIntel, 0, world);
    };
    private:
    std::vector<object*> world = {};
    BVHNode* BVH = nullptr;

};

#endif