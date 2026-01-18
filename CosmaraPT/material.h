#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vector3.h"
#include "Color.h"
#include "ray.h"
#include "object.h"
#include "texture.h"

enum MATERIALTYPE {
    LAMBERTIAN,
    LIGHT,
    METAL,
    DIELECTRIC,
    GLOSSY,
    UNKNOWN
};

struct textureDescriptor {
    texture* diffuse;
    texture* specular;
    texture* normal;
    texture* emissive;
    texture* roughness;
};

enum REFLECTIONTYPE {
    DIFFUSE,
    SPECULAR
};

class material {
    public:
    MATERIALTYPE mat_name = UNKNOWN;

    material() = default;

    material(Color _attenuation, Color _emissive):
    albedo(_attenuation), emissive(_emissive){}

    material(Color _attenuation, Color _emissive, textureDescriptor* _imag):
    albedo(_attenuation), emissive(_emissive), imag(_imag){}

    ~material(){};

    virtual bool generateRayAndAttenuation(const ray& incident, hitDescriptor* hitstream, ray* scatteredRay) {
        return false;
    }

    virtual double BRDF(const ray& incident, const ray& emergent, hitDescriptor* hitstream){
        return 1;
    };

    virtual double PDF(const ray& incident, const ray& emergent, hitDescriptor* hitstream){
        return 1;
    }

    virtual Color retrieveAlbedo() const {
        return albedo;
    }

    virtual Color retrieveEmissiveComponent() const {
        return emissive;
    }

    virtual MATERIALTYPE getMatName() const {
        return mat_name;
    }

    Color bilinearSample(texture* _texture, double u, double v, std::vector<std::vector<Color>>* _textureColors){
        double x = u*_texture->width-1;
        double y = v*_texture->height-1;
        
        double fracX = x-floor(x);
        double fracY = y-floor(y);

        Color topleft = (*_textureColors)[floor(y)][floor(x)];
        Color topright = (*_textureColors)[floor(y)][ceil(x)];
        Color bottomleft = (*_textureColors)[ceil(y)][floor(x)];
        Color bottomright = (*_textureColors)[ceil(y)][ceil(x)];

        Color interpolatedX = (topleft*(1-fracX)+fracX*topright);
        Color interpolatedY = (bottomleft*(1-fracY)+fracY*bottomright);

        Color finalColor = interpolatedX*(1-fracY) + interpolatedY*fracY;
        return finalColor;
    }

    void applyTextureData(material* self, hitDescriptor* hitstream, REFLECTIONTYPE reflector){
        textureDescriptor* texturedata = self->retrieveTextureDescriptor();
        if (!texturedata){
            hitstream->true_attenuation = self->retrieveAlbedo();
            hitstream->emissive = self->retrieveEmissiveComponent();
            return ;
        }
        if (reflector == DIFFUSE){
            hitstream->true_attenuation = (texturedata->diffuse) ? bilinearSample(texturedata->diffuse, hitstream->u,hitstream->v, &texturedata->diffuse->colorData):self->retrieveAlbedo();
        }
        else {
            hitstream->true_attenuation = (texturedata->specular) ? bilinearSample(texturedata->specular, hitstream->u,hitstream->v, &texturedata->specular->colorData):self->retrieveAlbedo();
        }
        hitstream->emissive = (texturedata->emissive)?bilinearSample(texturedata->emissive, hitstream->u, hitstream->v, &texturedata->emissive->colorData):self->retrieveEmissiveComponent();
        if (texturedata->normal){
            Color normalCoords = bilinearSample(texturedata->normal, hitstream->u, hitstream->v, &texturedata->normal->colorData);
            double x = normalCoords.x();
            double y = normalCoords.y();
            double z = normalCoords.z();

            x = (x*2.0)-1.0f;
            y = (y*2.0)-1.0f;
            z = (z*2.0)-1.0f;

            Vector3 arb = (fabs(hitstream->normal.x()) > 0.1f) ? Vector3(0.0, 1.0f, 0.0f): Vector3(1.0f, 0.0f, 0.0f);
            Vector3 tangent = cross(hitstream->normal, arb);
            Vector3 bitangent = cross(tangent, hitstream->normal);
            hitstream->normal = tangent*x + bitangent*y + hitstream->normal*z;
        }
    }

    double fresnel(double cosangle, double reflectivity){
        return reflectivity + (1-reflectivity) * pow((1-cosangle), 5);
    }

    virtual textureDescriptor* retrieveTextureDescriptor(){
        return imag;
    }

    private:
    Color albedo;
    Color emissive;
    textureDescriptor* imag;
};

#endif