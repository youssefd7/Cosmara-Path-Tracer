#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <thread>
#include <cstdint>
#include <unistd.h>
#include <chrono>
#include "Vector3.h"
#include "Color.h"
#include "utils.h"
#include "ray.h"
#include "scene.h"
#include "object.h"

enum skyboxType {
    GRADIENT,
    FILL
};


struct cameraDescriptor {
    double aspectratio;
    unsigned short height;
    unsigned short threadcount;
    Vector3 cameraCenter;
    Vector3 viewDirection;
    Vector3 upVector;
    double distanceFromViewport = 1.0f;
    double verticalFieldOfView = 90;
    double samplesPerPixel = 10;
    double gamma;
    scene* world;
    Color skyColorM;
    Color skyColorA;
    skyboxType type;
};

class camera {
    public:
    double aspectratio = 1;

    unsigned short height = 1000;
    unsigned short width = height*aspectratio;
    unsigned short linesCompleted;
    unsigned long pixelsCompleted;

    explicit camera(){

    }

    explicit camera(cameraDescriptor cameraInfo){
        height = cameraInfo.height;
        aspectratio = cameraInfo.aspectratio;
        width = aspectratio*height;
        cameraCenter = cameraInfo.cameraCenter;
        viewDirection = cameraInfo.viewDirection;
        upVector = cameraInfo.upVector;
        verticalFieldOfView = cameraInfo.verticalFieldOfView;
        distanceFromViewport = cameraInfo.distanceFromViewport;
        samplesPerPixel = cameraInfo.samplesPerPixel;
        world = cameraInfo.world;
        skyColorM = cameraInfo.skyColorM;
        skyColorA = cameraInfo.skyColorA;
        type = cameraInfo.type;
        gamma = cameraInfo.gamma;
        threadcount = cameraInfo.threadcount;
        colorBuffer.resize(height, std::vector<Color>(width));
    }

    ~camera(){
        delete image;
    }


    void initializeCam(){
        fillColorBuffer();
        calculateThreadRegions(threadcount, &threadMapping);
        inputFileHeaders();
        setupViewport();
    }

    double NDCfromPixel(unsigned short alpha, unsigned short limit) const{
        return double(alpha)/limit;
    }

    void render(){
        std::cout << "Rendering of scene initiated\n";
        for (unsigned short i = 0; i < height; ++i){
            double yNDC = NDCfromPixel(i, height);
            std::cout << "Lines left to render: " << height-i << (char)10;
            for (unsigned short j = 0; j < width; j++){
                Color pathColor = Color(0,0,0);
                double xNDC = NDCfromPixel(j, width);
                for (unsigned long sample = 0; sample < samplesPerPixel; sample++){
                    ray pathRay = retrieveRayFromNDC(xNDC, yNDC);
                    pathColor += TracePath(pathRay, 0, UNKNOWN);
                }
                writeToStream(image, pathColor, samplesPerPixel, gamma);
            }
        }
        std::cout << "Finished rendering scene\n";
    }

    void render(unsigned short b1, unsigned short b2){
        for (unsigned short i = b1; i < b2; ++i){
            double yNDC = NDCfromPixel(i, height);
            for (unsigned short j = 0; j < width; j++){
                Color pathColor = Color(0,0,0);
                double xNDC = NDCfromPixel(j, width);
                for (unsigned long sample = 0; sample < samplesPerPixel; sample++){
                    ray pathRay = retrieveRayFromNDC(xNDC, yNDC);
                    Color singlePathColor = TracePath(pathRay, 0, UNKNOWN);
                    pathColor += singlePathColor;
                }
                pathColor = pathColor * 1.0f/samplesPerPixel;
                colorBuffer[i][j] = pathColor;
                pixelsCompleted += 1;
            }
            linesCompleted += 1;
        }
    }

    void showTotalProgress(){
        while (linesCompleted < height){
            std::cout << "Percentage Rendered: " << clamp((((double)pixelsCompleted/(double)(height*width)))*100, 0, 100)  << "% | ";
            std::cout << "Pixel Progress: " << pixelsCompleted << "/" << (height*width) << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << "Progress complete!" << std::endl;
    }

    void multiThreadedRender(){
        threads.clear();
        for (unsigned short threadnum = 1; threadnum <= threadcount; ++threadnum){
            unsigned short interval1 = 0;
            unsigned short interval2 = height;
            retrieveIntervalFromThread(threadnum, &interval1, &interval2);
            threads.emplace_back([this, interval1, interval2]() {
            render(interval1, interval2);
            });
        }
        threads.emplace_back(std::bind(&camera::showTotalProgress, this));
        std::cout << "Waiting for threads to complete..." << std::endl;
        for (auto &thread : threads){
            if (thread.joinable()){
                thread.join();
            }
        }
        std::cout << "All threads completed." << std::endl;
        writeBufferToStream(image, &colorBuffer, gamma);
    }

    
    private:
    Vector3 cameraCenter;
    Vector3 viewDirection;
    Vector3 upVector;
    Vector3 rightVector;
    Vector3 trueUp;
    Vector3 viewportX;
    Vector3 viewportY;

    Vector3 topright;
    Vector3 topleft;
    Vector3 bottomright;
    Vector3 bottomleft;

    double viewportHeight;
    double viewportWidth;
    double distanceFromViewport = 1.0f;
    double verticalFieldOfView = 90;
    double gamma;
    unsigned short maxdepth = 10;
    unsigned short threadcount = 1;
    unsigned long samplesPerPixel;
    std::vector<unsigned short> threadMapping;
    std::vector<std::vector<Color>> colorBuffer;
    std::vector<std::thread> threads;

    Color skyColorM = Color(202, 244, 255) * 1.0f/255;
    Color skyColorA = Color(135, 206, 235) * 1.0f/255;
    skyboxType type;

    scene* world;

    std::ofstream *image = new std::ofstream("image.ppm");


    void calculateThreadRegions(unsigned short threadnumber, std::vector<unsigned short>* threadMapping){
        threadMapping->push_back(0);
        unsigned short interval = height/threadnumber;
        unsigned short remainder = height - threadnumber*interval;
        for (unsigned short i = 1; i < threadnumber; ++i){
            threadMapping->push_back(interval*i);
        }
        threadMapping->push_back(height);
    }


    void inputFileHeaders(){
        *image << "P3\n" << std::to_string(width) << " " << std::to_string(height) << (char)10 << "255\n";
    }
    
    void setupViewport(){
        viewDirection = unit(viewDirection);
        upVector = unit(upVector);

        rightVector = cross(viewDirection, upVector);
        trueUp = cross(viewDirection, rightVector);
        
        double VFOVScalingFactor = tan(rad(verticalFieldOfView/2.0f));
        viewportHeight = 2*distanceFromViewport*VFOVScalingFactor;
        viewportWidth = aspectratio*viewportHeight;

        Vector3 viewportPlaneCenter = cameraCenter + viewDirection * distanceFromViewport;
        viewportX = rightVector * viewportWidth/2.0f;
        viewportY = trueUp * viewportHeight/2.0f;

        topright = viewportPlaneCenter + viewportY + viewportX;
        topleft = viewportPlaneCenter + viewportY - viewportX;
        bottomright = viewportPlaneCenter - viewportY + viewportX;
        bottomleft = viewportPlaneCenter - viewportY - viewportX;
    }

    ray retrieveRayFromNDC(double xC, double yC){
        double xSample = xC + random_dbl_range(-0.5f/(width),0.5f/(width));
        double ySample = yC + random_dbl_range(-0.5f/(height),0.5f/(height));
        Vector3 rayDirection = (bottomleft + xSample*(bottomright-bottomleft) + 
        ySample*(topright-bottomright)) - cameraCenter;
        ray currentRay(cameraCenter, unit(rayDirection));
        return currentRay;
    }

    void retrieveIntervalFromThread(unsigned short thread, unsigned short* b1, unsigned short* b2){
        if (thread == 1){*b1 = 0;*b2=threadMapping[1];}
        else if (thread == threadcount){*b1 = threadMapping[threadMapping.size()-2];*b2 = threadMapping[threadMapping.size()-1];}
        *b1 = threadMapping[(thread)-1];*b2 = threadMapping[(thread)];
    }

    void fillColorBuffer(){
        for (unsigned short i = 0; i < height; ++i){
            for (unsigned short j = 0; j < width; ++j){
                colorBuffer[i][j] = Color(0,0,0);
            }
        }
    }

    Color TracePath(ray& currentRay, unsigned short depth, MATERIALTYPE previous_mat){
        if (previous_mat == 1){return Color(1,1,1);}
        if (depth >= maxdepth){return Color(0,0,0);}
        Vector3 rayDirection = unit(currentRay.get_direction());
        hitDescriptor hitRecord;
        if (world->intersectWorldBVH(currentRay, &hitRecord)){
            material* material_hit = hitRecord.hitMaterial;
            ray scatteredRay;
            bool scattered = material_hit->generateRayAndAttenuation(currentRay, &hitRecord, &scatteredRay);
            Vector3 normal = hitRecord.normal;
            if (scattered){
                //double maxattenuation = std::max(hitRecord.true_attenuation.x(), std::max(hitRecord.true_attenuation.y(),hitRecord.true_attenuation.z()));
                double cosine_theta = dot(normal, scatteredRay.get_direction());
                double BRDF = hitRecord.brdf;
                double PDF = hitRecord.pdf;
                Color incomingIlluminance = TracePath(scatteredRay, depth+1, material_hit->getMatName());
                Color totalIlluminance = hitRecord.emissive + ((BRDF/PDF) * hitRecord.true_attenuation * incomingIlluminance * cosine_theta);
                return totalIlluminance;
            }
        }
        if (type == GRADIENT){
            double yPositionRay = (rayDirection.y()+1)/2.0f;
            return yPositionRay*skyColorM +(1-yPositionRay)*skyColorA;
        } else if (type == FILL){
            return skyColorM;
        }
        else {
            return (skyColorM+skyColorA)*0.5f;
        }
    }
};


#endif