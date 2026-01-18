#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "object.h"
#include <unistd.h>

class BVHNode {
    public:
    BVHNode() = default;
    unsigned short maxPrimitive = 8;
    AABB box;
    BVHNode* left;
    BVHNode* right;
    int startIndex, endIndex;
    std::vector<object*> primitivesArr;
    bool isALeaf = false;
    bool isLeaf(){
        return isALeaf;
    }

    BVHNode* buildBVH(std::vector<object*>& primitives, int start, int end, int depth){
        startIndex = start;endIndex=end;
        //this->primitivesArr.assign(primitives.begin()+start, primitives.begin()+end);

        if (end-start <= maxPrimitive){
            AABB boundBox = primitives[start]->getAABB();
            for (unsigned long i = start+1; i < end; ++i){
                AABB currentbox = primitives[i]->getAABB();
                boundBox = makeBox(boundBox, currentbox);
            }
            this->box = boundBox;
            this->isALeaf = true;
            return this;
        }

        else {
            AABB boundBox = primitives[start]->getAABB();
            for (unsigned long i = start+1; i < end; ++i){
                AABB currentbox = primitives[i]->getAABB();
                boundBox = makeBox(boundBox, currentbox);
            }

            int axis = boundBox.findLongestAxis();
            int mid = (start+end)/2;

            std::sort(primitives.begin()+start, primitives.begin()+end, [=](object* prim1, object* prim2){
                if (axis == 0){
                    return (prim2->getCenter().x() - prim1->getCenter().x()) > EPSILON;
                }
                else if (axis == 1){
                    return prim1->getCenter().y() > prim2->getCenter().y();
                }
                else {
                    return prim1->getCenter().z() > prim2->getCenter().z();
                }
            });

            this->left = new BVHNode;
            this->right = new BVHNode;

            this->left->buildBVH(primitives, start, mid, depth+1);
            this->right->buildBVH(primitives, mid, end, depth+1);
            this->box = boundBox;
        }
        return this;
    }


    bool intersect(const ray& incident, hitDescriptor* hitIntel, int depth, std::vector<object*>& primitives) {
        if (!this->box.intersect(incident)){
            return false;
        }
        if (this->isLeaf()){
            bool intersected = false;
            double closestroot = infinity;
            hitDescriptor closestHit;
            for (unsigned long i = startIndex; i < endIndex; ++i){
                if (primitives[i]->intersect(incident, hitIntel)){
                    if (!intersected){intersected = true;}
                    if (hitIntel->trueroot < closestroot){
                        closestroot = hitIntel->trueroot;
                        closestHit = *hitIntel;
                    }
                }
            }
            *hitIntel = closestHit;
            return intersected;
        }
        bool hitleft;
        bool hitright;
        hitDescriptor leftHitD, rightHitD;

        if (this->left){
            hitleft = this->left->intersect(incident, &leftHitD, depth+1, primitives);
        }

        if (this->right){
            hitright = this->right->intersect(incident, &rightHitD, depth+1, primitives);
        }
        if (hitleft && hitright){
            if (leftHitD.trueroot > rightHitD.trueroot){
                *hitIntel = rightHitD;
            }
            else {
                *hitIntel = leftHitD;
            }
            return true;
        }
        else if (hitleft){
            *hitIntel = leftHitD;
            return true;
        }
        else if (hitright){
            *hitIntel = rightHitD;
            return true;
        }
        else {}
        return false;
    }

    static AABB makeBox(AABB& box1, AABB& box2){
        Vector3 min(fmin(box1.pointmin.x(), box2.pointmin.x()),
        fmin(box1.pointmin.y(), box2.pointmin.y()),
        fmin(box1.pointmin.z(), box2.pointmin.z()));
        Vector3 max(fmax(box1.pointmax.x(), box2.pointmax.x()),
        fmax(box1.pointmax.y(), box2.pointmax.y()),
        fmax(box1.pointmax.z(), box2.pointmax.z()));
        AABB out = AABB(min, max);
        return out;
    }
    private:
    hitDescriptor* closest = nullptr;
};

#endif