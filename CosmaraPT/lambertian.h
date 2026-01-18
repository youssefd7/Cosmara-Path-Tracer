#pragma once

#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "material.h"
#include "utils.h"

class lambertian : public material {
    public:
    MATERIALTYPE mat_name = LAMBERTIAN;

    lambertian(Color _albedo, Color _emissive):albedo(_albedo), emissive(_emissive){
        grayscale = ((albedo.x()+albedo.y()+albedo.z())/3.0f)*(1.0f/PI);
    }
    lambertian(Color _albedo, Color _emissive, textureDescriptor* _imag):albedo(_albedo), emissive(_emissive), imag(_imag){
        grayscale = ((albedo.x()+albedo.y()+albedo.z())/3.0f)*(1.0f/PI);
    }

    bool generateRayAndAttenuation(const ray& incident, hitDescriptor* hitstream, ray* scatteredRay) override{
        applyTextureData(this,hitstream, DIFFUSE);
        DISTRIBUTION sampletype = COSINE;
        Vector3 scatteredDirection = unit(generateDirInHemisphere(hitstream->normal, sampletype));
        *scatteredRay = ray(hitstream->point, scatteredDirection);
        hitstream->brdf = BRDF(incident, *scatteredRay, hitstream);
        hitstream->pdf = (sampletype == COSINE) ? PDF(incident, *scatteredRay, hitstream):1.0f/(2*PI);
        return true;
    }

    double BRDF(const ray& incident, const ray& emergent, hitDescriptor* hitstream) override{
        return grayscale;
    };

    double PDF(const ray& incident, const ray& emergent, hitDescriptor* hitstream) override{
        return dot(emergent.get_direction(), hitstream->normal)/PI;
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
    Color albedo;
    Color emissive;
    double grayscale;
    textureDescriptor* imag;
};


#endif