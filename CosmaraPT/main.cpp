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
#include "scenetools.h"
#include "SceneRender.h"

unsigned short threadcount = 1;
unsigned short spp = 10;
double gamma = 2.2;
cameraDescriptor cameraData;

Vector3 cameraCenter = Vector3(0,0,0);
Color skyColorM;
Color skyColorA;

Vector3 ViewDirectionFromLookAt(Vector3 center, Vector3 lookat){
    return lookat-center;
}

void initializeCamera(cameraDescriptor* cameraInfo, scene* sceneptr){
    cameraInfo->height = 700;
    cameraInfo->aspectratio = cameraInfo->aspectratio;
    //cameraInfo->cameraCenter = cameraCenter;
    cameraInfo->viewDirection = ViewDirectionFromLookAt(cameraCenter, Vector3(0,0,-1));
    cameraInfo->upVector = Vector3(0,1,0);
    cameraInfo->verticalFieldOfView = 45;
    cameraInfo->distanceFromViewport = 1;
    cameraInfo->samplesPerPixel = spp;
    cameraInfo->world = sceneptr;
    //cameraInfo->skyColorM = skyColorM;
    //cameraInfo->skyColorA = skyColorA;
    cameraInfo->type = GRADIENT;
    cameraInfo->gamma = gamma;
    cameraInfo->threadcount = threadcount;
}

void CornellBox(scene* sceneptr, std::vector<material*>* materialQueue, cameraDescriptor* desc){
    double scalefactor = 1.0f;
    sceneptr->clear();
    materialQueue->clear();
    desc->aspectratio = 1.0f;
    desc->skyColorM = Color(0,0,0);
    desc->skyColorA = Color(0,0,0);
    desc->cameraCenter = Vector3(0,0,50/scalefactor);

    textureDescriptor* texturetest = new textureDescriptor;
    //texture* imag = new texture("assets/texture.ppm", gamma);
    //texture* emit = new texture("assets/emissive.ppm", gamma);
    //texture* normal = new texture("assets/normal.ppm");

    //texturetest->diffuse = imag;
    //texturetest->emissive = emit;
    //texturetest->normal = normal;

    lambertian* diffuseMatGround = new lambertian(Color(244, 232, 226)*1.0f/255, Color(0,0,0));
    lambertian* diffuseMatBall = new lambertian(Color(244, 232, 226)*1.0f/255, Color(0,0,0), texturetest);
    lambertian* boxmat = new lambertian(Color(105, 105, 105)*1.0f/255, Color(0,0,0), texturetest);
    lambertian* diffuseMatRed = new lambertian(Color(1,0.15,0.12), Color(0,0,0));
    lambertian* diffuseMatGreen = new lambertian(Color(0.12,1,0.15), Color(0,0,0));
    lambertian* diffuseMatBlue = new lambertian(Color(0.12,0.15,1), Color(0,0,0));

    lambertian* diffuseMatSphere1 = new lambertian(Color(0.12,0.15,1), Color(0,0,0));
    lambertian* diffuseMatSphere2 = new lambertian(Color(1,0.15,0.12), Color(0,0,0));
    lambertian* diffuseMatSphere3 = new lambertian(Color(0.12,1,0.15), Color(0,0,0));
    metal* metallic = new metal(Color(1,1,1), Color(0,0,0), 0, 1.0f);
    dielectric* glass = new dielectric(Vector3(180,180,180)*1.0f/255, Vector3(0,0,0), 1.5);
    glossy* gloss = new glossy(Vector3(0.5,0.5,0.5), Vector3(0,0,0), 0.6f, 0.05);
    
    light* mainLight = new light(Color(1,1,1)*20, 2*PI);

    double depth = -36.0f/scalefactor;
    double planeWidth = 20/scalefactor;
    double planeHeight = 20/scalefactor;

    plane* back = new plane(Vector3(-planeWidth,-planeHeight,depth), Vector3(planeWidth*2,0,0), Vector3(0,planeHeight*2,0), diffuseMatGround);
    plane* front = new plane(Vector3(-planeWidth,-planeHeight,depth+planeWidth*2), Vector3(planeWidth*2,0,0), Vector3(0,planeHeight*2,0), diffuseMatGround);
    plane* leftSide = new plane(Vector3(-planeWidth, -planeHeight, depth), Vector3(0,planeHeight*2,0), Vector3(0,0,planeWidth*2), diffuseMatRed);
    plane* rightSide = new plane(Vector3(planeWidth, planeHeight, depth), Vector3(0,-planeHeight*2,0), Vector3(0,0, planeWidth*2), diffuseMatGreen);
    plane* ceiling = new plane(Vector3(planeWidth, planeHeight, depth), Vector3(0,0,planeWidth*2), Vector3(-planeWidth*2, 0, 0), diffuseMatGround);
    plane* floor = new plane(Vector3(-planeWidth, -planeHeight, depth), Vector3(0,0,planeWidth*2), Vector3(planeWidth*2, 0, 0), diffuseMatGround);
    plane* planelight = new plane(Vector3(planeWidth*0.25, planeHeight*0.995, depth+0.35*(planeWidth*2)), Vector3(0,0,planeWidth*2-0.7*(planeWidth*2)), Vector3(-planeWidth*0.5, 0, 0), mainLight);
    sphere* sphereMetal = new sphere(0.3, Vector3(0.6,-0.25+0.3,-0.85), metallic);
    sphere* sphereGlass = new sphere(0.3, Vector3(0.3,-planeHeight+0.3,-0.5), glass);
    sphere* sphereDiffuse = new sphere(7.5, Vector3(-10,-20+7.5,-25.0), gloss);
    //createBox(sceneptr, Vector3(-0.3*planeHeight, -planeHeight, 0.25/1.8f*depth), Vector3(0.3*planeHeight, -planeHeight+0.6*planeHeight, 1.25/1.8f*depth), diffuseMatGround);

    sceneptr->addObject(back);
    sceneptr->addObject(leftSide);
    sceneptr->addObject(rightSide);
    sceneptr->addObject(ceiling);
    sceneptr->addObject(floor);
    sceneptr->addObject(planelight);
    //sceneptr->addObject(front);
    //sceneptr->addObject(sphereMetal);
    //sceneptr->addObject(sphereDiffuse);
    //sceneptr->addObject(sphereGlass);
    //sceneptr->addObject(tri);

    loadModel(sceneptr, "assets/dragon.obj", gloss, true);

    materialQueue->push_back(diffuseMatGround);
    materialQueue->push_back(diffuseMatBall);
    materialQueue->push_back(boxmat);
    materialQueue->push_back(diffuseMatRed);
    materialQueue->push_back(diffuseMatGreen);
    materialQueue->push_back(diffuseMatBlue);

    materialQueue->push_back(diffuseMatSphere1);
    materialQueue->push_back(diffuseMatSphere2);
    materialQueue->push_back(diffuseMatSphere3);
    materialQueue->push_back(metallic);
    materialQueue->push_back(glass);
    materialQueue->push_back(mainLight);    
    materialQueue->push_back(gloss); 
}

int main(int argc, char** argv){
    if (argc > 0){
        threadcount = atoi(argv[1]);
        spp = atoi(argv[2]);
    }
    std::vector<material*> materialFreeQueue;
    std::vector<texture*> textureFreeQueue;  
    std::chrono::steady_clock clock;
    auto start1 = clock.now();

    std::cout << "Loading Assets..." << (char)10;

    scene currentSceneObject;
    scene* currentScene = &currentSceneObject;
    textureDescriptor* texturetest = new textureDescriptor;

    sceneLayout(currentScene, &materialFreeQueue, &cameraData);
    initializeCamera(&cameraData, currentScene);
    camera* cam = new camera(cameraData);
    cam->initializeCam();
    currentScene->initBVH();

    std::cout << "Finished Loading Assets!" << (char)10;
    auto end1 = clock.now();
    auto time_span_assets = static_cast<std::chrono::duration<double>>(end1-start1);
    std::cout << "Asset loading duration: " << time_span_assets.count() << " seconds" << std::endl;

    auto start2 = clock.now();

    cam->multiThreadedRender();

    auto end2 = clock.now();
    auto time_span_render = static_cast<std::chrono::duration<double>>(end2-start2);
    std::cout << "Time taken to render scene: " << time_span_render.count() << " seconds" << std::endl;

    for (material* freedmat:materialFreeQueue){
        delete freedmat;
    }

    for (texture* freedtexture:textureFreeQueue){
        delete freedtexture;
    }


    delete cam;
}