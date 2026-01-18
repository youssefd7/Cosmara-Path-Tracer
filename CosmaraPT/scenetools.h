#ifndef SCENE_TOOLS_H
#define SCENE_TOOLS_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include "Vector3.h"
#include "camera.h"
#include "scene.h"
#include "sphere.h"
#include "material.h"
#include "lambertian.h"
#include "light.h"
#include "metal.h"
#include "dielectric.h"
#include "plane.h"
#include "time.h"
#include <unistd.h>
#include <math.h>
#include <thread>
#include <type_traits>
#include "texture.h"
#include "glossy.h"
#include "aabb.h"
#include "triangle.h"
#include "modelreader.h"
#include "modeltools.h"
#include "model.h"


void loadModel(scene* currentScene, const char* path, material* modelmat, bool mode){
    double scalefactor = 1.0f;
    ModelReader reader(path);
    Vertex* vertexBuffer = (Vertex*)malloc(sizeof(Vertex)*reader.triangleCount*3);
    reader.initReader(path);
    reader.initArray(vertexBuffer);
    modeltools tools(&reader);
    std::vector<object*> modelData;
    for (unsigned long i = 0; i < reader.triangleCount*3; i+=3){
        triangle* tri = new triangle(
        Vector3(vertexBuffer[i].vertex1, vertexBuffer[i].vertex2, vertexBuffer[i].vertex3),
        Vector3(vertexBuffer[i+1].vertex1, vertexBuffer[i+1].vertex2, vertexBuffer[i+1].vertex3),
        Vector3(vertexBuffer[i+2].vertex1, vertexBuffer[i+2].vertex2, vertexBuffer[i+2].vertex3),
        modelmat);
        modelData.emplace_back(tri);
    }
    Vector3 centerTarget(0,0,-16);
    double bottomCoord = -20.0f;
    double length = 20.75f;
    AABB modelBox = model::computeBoundingVolume(modelData);
    Vector3 modelCenter = (modelBox.pointmin + modelBox.pointmax)/2.0f;
    double distFromNil = -std::min(modelBox.pointmin.y(), modelBox.pointmax.y());
    double scalefactor2 = length/(mode ? abs(modelBox.pointmax.x() - modelBox.pointmin.x()): abs(modelBox.pointmax.y() - modelBox.pointmin.y()));
    centerTarget = Vector3(centerTarget.x(), bottomCoord+abs(modelBox.pointmax.y()+modelBox.pointmin.y())*scalefactor2/2.0f + distFromNil * scalefactor2 ,centerTarget.z());
        
    tools.scale(scalefactor2);
    tools.applyPitch(0);
    tools.translate(centerTarget - modelCenter*scalefactor2);

    modelData.clear();

    for (unsigned long i = 0; i < reader.triangleCount*3; i+=3){
        triangle* tri = new triangle(
        Vector3(vertexBuffer[i].vertex1, vertexBuffer[i].vertex2, vertexBuffer[i].vertex3),
        Vector3(vertexBuffer[i+1].vertex1, vertexBuffer[i+1].vertex2, vertexBuffer[i+1].vertex3),
        Vector3(vertexBuffer[i+2].vertex1, vertexBuffer[i+2].vertex2, vertexBuffer[i+2].vertex3),
        modelmat);
        modelData.emplace_back(tri);
    }

    currentScene->reserveObjectCapacity(reader.triangleCount);
    model* modelFile = new model(modelData, modelmat);
    currentScene->addObject(modelFile);
    free(vertexBuffer);
}

void createBox(scene* currentScene, Vector3 start, Vector3 end, material* mat){
    Vector3 min = Vector3(fmin(start.x(), end.x()), fmin(start.y(), end.y()), fmin(start.z(), end.z()));
    Vector3 max = Vector3(fmax(start.x(), end.x()), fmax(start.y(), end.y()), fmax(start.z(), end.z()));

    Vector3 deltaX = Vector3(max.x()-min.x(), 0, 0);
    Vector3 deltaY = Vector3(0, max.y()-min.y(), 0);
    Vector3 deltaZ = Vector3(0, 0, max.z()-min.z());

    plane* back = new plane(Vector3(min.x(), min.y(), min.z()), deltaX, deltaY, mat);
    plane* front = new plane(Vector3(min.x(), min.y(), max.z()), deltaX, deltaY, mat);
    plane* ceiling = new plane(Vector3(max.x(), max.y(), max.z()), -deltaZ, -deltaX, mat);
    plane* floor = new plane(Vector3(max.x(), min.y(), min.z()), deltaZ, -deltaX, mat);
    plane* side1 = new plane(Vector3(max.x(), min.y(), max.z()), deltaY, -deltaZ, mat);
    plane* side2 = new plane(Vector3(min.x(), max.y(), max.z()), -deltaY, -deltaZ, mat);

    currentScene->addObject(back);
    currentScene->addObject(front);
    currentScene->addObject(ceiling);
    currentScene->addObject(floor);
    currentScene->addObject(side1);
    currentScene->addObject(side2);
}

#endif