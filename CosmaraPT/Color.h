#pragma once

#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <math.h>
#include <vector>
#include "Vector3.h"
#include "utils.h"

using Color = Vector3;

double gammaCorrectRGBComponent(double gamma, double RGB){
    return pow(RGB, 1.0f/gamma);
}

Color convertRatioToRGB(Color& in, double gamma){
    auto scaler = [=](double scalefactor, double ratio){return floor(scalefactor*gammaCorrectRGBComponent(gamma, clamp(ratio,0,1)));};
    double scalefactor = 255;
    return Color(scaler(scalefactor, in.x()), scaler(scalefactor, in.y()), scaler(scalefactor, in.z()));
}

void writeToStream(std::ostream* stream, Color& in, unsigned long samplesPerPixel, double gamma){
    Color finalColor = in*(1.0f/samplesPerPixel);
    *stream << convertRatioToRGB(finalColor, gamma);
}

void writeBufferToStream(std::ostream* stream, std::vector<std::vector<Color>>* colorBuffer, double gamma){
    for (unsigned long i = 0; i < colorBuffer->size(); ++i){
        for (unsigned long j = 0; j < (*colorBuffer)[i].size(); ++j){
            Color finalColor = (*colorBuffer)[i][j];
            *stream << convertRatioToRGB(finalColor, gamma);
        }
    }

}

#endif