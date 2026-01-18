#pragma once


//Perfect Specular Reflection

#ifndef METAL_H
#define METAL_H

#include "material.h"
#include "utils.h"

class metal : public material {
    public:
    MATERIALTYPE mat_name = METAL;

    metal(Color _albedo, Color _emissive, double _roughness, double _reflectance):
    albedo(_albedo), emissive(_emissive), roughness(_roughness), reflectance(_reflectance){}
    metal(Color _albedo, Color _emissive, double _roughness, double _reflectance, textureDescriptor* _imag):
    albedo(_albedo), emissive(_emissive), roughness(_roughness), reflectance(_reflectance), imag(_imag){
    }

    bool generateRayAndAttenuation(const ray& incident, hitDescriptor* hitstream, ray* scatteredRay) override{
        applyTextureData(this,hitstream, SPECULAR);
        Vector3 reflectedVector = reflect(incident.get_direction(), hitstream->normal);
        Vector3 scatteredDirection = unit(reflectedVector + roughness*generateDirInHemisphere(reflectedVector, UNIFORM));
        *scatteredRay = ray(hitstream->point, scatteredDirection);
        hitstream->brdf = BRDF(incident, *scatteredRay, hitstream);
        hitstream->pdf = PDF(incident, *scatteredRay, hitstream);
        return dot(scatteredDirection, hitstream->normal) > 0;
    }

    double BRDF(const ray& incident, const ray& emergent, hitDescriptor* hitstream) override{
        double cosAngleOfIncidence = dot(-incident.get_direction(), hitstream->normal);
        double fresnelfactor = fresnel(cosAngleOfIncidence, reflectance);
        return fresnelfactor/abs(cosAngleOfIncidence);
    };

    double PDF(const ray& incident, const ray& emergent, hitDescriptor* hitstream) override{
        return 1.0f;
    };

    Color retrieveAlbedo() const override{
        return albedo;
    }
    
    Color retrieveEmissiveComponent() const override{
        return emissive;
    }

    MATERIALTYPE getMatName() const override{
        return mat_name;
    }

    textureDescriptor* retrieveTextureDescriptor() override{
        return imag;
    }

    private:
    double reflectance = 1.0f;
    Color albedo;
    Color emissive;
    double roughness;
    textureDescriptor* imag;
};


#endif