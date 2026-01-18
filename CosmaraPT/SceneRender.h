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

void sceneLayout(scene* sceneptr, std::vector<material*>* materialQueue, cameraDescriptor* camData){
    double scalefactor = 1.0f;
    sceneptr->clear();
    materialQueue->clear();
    camData->aspectratio = 1.0f;
    camData->skyColorM = Color(135, 206, 235)*0.0f/255.0;
    camData->skyColorA = Color(135, 206, 235)*0.0f/255.0;
    camData->cameraCenter = Vector3(0,0,60/scalefactor);
    
    textureDescriptor* texturetest = new textureDescriptor;
    double gamma = 2.2;
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
    lambertian* diffuseMatBlue = new lambertian(Color(0.2, 0.4, 0.8), Color(0,0,0));

    lambertian* diffuseMatSphere1 = new lambertian(Color(0.12,0.15,1), Color(0,0,0));
    lambertian* diffuseMatSphere2 = new lambertian(Color(1,0.15,0.12), Color(0,0,0));
    lambertian* diffuseMatSphere3 = new lambertian(Color(0.12,1,0.15), Color(0,0,0));
    metal* metallic = new metal(Color(1,1,1), Color(0,0,0), 0, 1.0f);
    dielectric* glass = new dielectric(Vector3(255, 255, 255)*1.0f/255, Vector3(0,0,0), 1.5);
    dielectric* diamond = new dielectric(Vector3(255,255,255)*1.0f/255, Vector3(0,0,0), 2.4);
    glossy* gloss = new glossy(Color(212, 175, 55)*0.7f/255.0, Vector3(0,0,0), 0.225f, 1.5);
    glossy* gloss2 = new glossy(Color(150,150,150)*1.0f/255.0, Vector3(0,0,0), 0.45f, 1.5);
    light* mainLight = new light(Color(1,1,1)*20, rad(180));

    double depth = -36.0f/scalefactor;
    double planeWidth = 20/scalefactor;
    double planeHeight = 20/scalefactor;

    plane* back = new plane(Vector3(-planeWidth,-planeHeight,depth), Vector3(planeWidth*2,0,0), Vector3(0,planeHeight*2,0), diffuseMatGround);
    plane* front = new plane(Vector3(-planeWidth,-planeHeight,depth+planeWidth*3), Vector3(planeWidth*2,0,0), Vector3(0,planeHeight*2,0), diffuseMatGround);
    plane* leftSide = new plane(Vector3(-planeWidth, -planeHeight, depth), Vector3(0,planeHeight*2,0), Vector3(0,0,planeWidth*2), diffuseMatRed);
    plane* rightSide = new plane(Vector3(planeWidth, planeHeight, depth), Vector3(0,-planeHeight*2,0), Vector3(0,0, planeWidth*2), diffuseMatGreen);

    plane* ceiling = new plane(Vector3(planeWidth, planeHeight, depth), Vector3(0,0,planeWidth*2), Vector3(-planeWidth*2, 0, 0), diffuseMatGround);
    plane* floor = new plane(Vector3(-planeWidth, -planeHeight, depth), Vector3(0,0,planeWidth*2), Vector3(planeWidth*2, 0, 0), diffuseMatGround);
    plane* planelight = new plane(Vector3(planeWidth*0.25, planeHeight*0.995, depth+0.35*(planeWidth*2)), Vector3(0,0,(planeWidth*2-0.7*(planeWidth*2))), Vector3(-planeWidth*0.5, 0, 0), mainLight);

    sphere* sphereGlass = new sphere(6.8, Vector3(10,-20+6.8,-14), glass);
    sphere* sphereMetal = new sphere(6.8, Vector3(-10,-20+6.8,-24), gloss2);
    sphere* sphereDiffuse = new sphere(6.8, Vector3(10,-20+6.8,-8.0), boxmat);

    sceneptr->addObject(back);
    sceneptr->addObject(leftSide);
    sceneptr->addObject(rightSide);
    sceneptr->addObject(ceiling);
    sceneptr->addObject(floor);
    sceneptr->addObject(planelight);

    loadModel(sceneptr, "assets/lucy.obj", gloss2, true);


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
