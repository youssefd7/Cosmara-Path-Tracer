#ifndef TEXTURE_H
#define TEXTURE_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "Color.h"

enum fileType {
    PPM,
    BMP
};

class texture {
    public:
    unsigned short height;
    unsigned short width;
    double maxval;
    double gamma;
    std::vector<std::vector<Color>> colorData;
    
    explicit texture(std::string filepath):path(filepath), gamma(1){
        texturefile = std::ifstream(filepath);
        retrieveHeaderData();
        retrieveColorData();
    }

    explicit texture(std::string filepath, double _gamma):path(filepath), gamma(_gamma){
        texturefile = std::ifstream(filepath);
        retrieveHeaderData();
        retrieveColorData();
    }

    std::vector<std::vector<Color>> getColorData(){
        return colorData;
    }

    private:

    std::string path;
    std::ifstream texturefile;

    void retrieveHeaderData(){
        std::string currentLine;
        std::string currentNum;
        std::getline(texturefile, currentLine);
        std::getline(texturefile, currentLine);
        for (char character:currentLine){
            if (character != ' '){currentNum += character;}
            else {
                width = std::stoi(currentNum);
                currentNum = "";
            }
        }
        height = std::stoi(currentNum);
        std::getline(texturefile, currentLine);
        maxval = std::stoi(currentLine);
    }
    template <typename T>
    void addColorFromLine(std::string currentString, std::vector<T>* colorBuffer){
        unsigned short state = 0;
        double r;
        double g;
        double b;
        std::string colorComp;
        for (char character:currentString){
            if (character != ' '){colorComp += character;}
            else {
                if ((state) == 0){
                    r = pow(std::stoi(colorComp)/maxval, gamma);
                }
                if ((state) == 1){
                    g = pow(std::stoi(colorComp)/maxval, gamma);
                }
                state+=1;
                colorComp = "";
            }
        }
        b = pow(std::stoi(colorComp)/maxval, gamma);
        colorBuffer->emplace_back(Color(r,g,b));
    }

    void retrieveColorData(){
        std::string currentString;
        for (unsigned short i = 0; i < height; ++i){
            std::vector<Color> row;
            for (unsigned short j = 0; j < width; ++j){
                std::getline(texturefile, currentString);
                addColorFromLine<Color>(currentString, &row);
            }
            colorData.emplace_back(row);
            row.clear();
        }
    }
};

#endif