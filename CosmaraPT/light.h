#pragma once

//Light header
#ifndef LIGHT_H
#define LIGHT_H

//Header and Module includes
#include <cmath>
#include "material.h"

class light : public material {
    public:
    MATERIALTYPE mat_name = LIGHT;

    explicit light(Color _emissive, double AOE):emissive(_emissive), AreaOfEffect(AOE){
        cosHalfAngle = cos(AreaOfEffect/2.0f);
    }

    bool generateRayAndAttenuation(const ray& incident, hitDescriptor* hitstream, ray* scatteredRay) override {
        Vector3 scatteredDirection = unit(hitstream->normal + generateDirInHemisphere(hitstream->normal, UNIFORM));
        double cosAngleOfIncidence = dot(incident.get_direction(), -hitstream->normal);
        bool emitted = (cosAngleOfIncidence >= cosHalfAngle) ? true:false;
        hitstream->true_attenuation = Vector3(1,1,1);
        hitstream->emissive = emissive;
        *scatteredRay = ray(hitstream->point, scatteredDirection);
        hitstream->brdf = 1.0f;
        hitstream->pdf = 1.0f;
        return emitted;
    }

    double BRDF(const ray& incident, const ray& emergent, hitDescriptor* hitstream) override {
        return 1.0f;
    }

    double PDF(const ray& incident, const ray& emergent, hitDescriptor* hitstream) override {
        return 1.0f;
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
    Color emissive;
    double AreaOfEffect;
    double cosHalfAngle;
    textureDescriptor* imag;
};

#endif