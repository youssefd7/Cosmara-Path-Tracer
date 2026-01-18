#pragma once

#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "material.h"
#include "utils.h"



class dielectric : public material {
    public:
    MATERIALTYPE mat_name = DIELECTRIC;
    dielectric(Color _albedo, Color _emissive, double _refractiveIndex):
    albedo(_albedo), emissive(_emissive), refractiveIndex(_refractiveIndex){
        inv_refractive = 1.0f/refractiveIndex;
        baseref = pow((1-_refractiveIndex)/(1+_refractiveIndex), 2);
    }

    dielectric(Color _albedo, Color _emissive, double _refractiveIndex, textureDescriptor* _imag):
    albedo(_albedo), emissive(_emissive), refractiveIndex(_refractiveIndex), imag(_imag){
        inv_refractive = 1.0f/refractiveIndex;
        baseref = pow((1-_refractiveIndex)/(1+_refractiveIndex), 2);
    }

    bool generateRayAndAttenuation(const ray& incident, hitDescriptor* hitstream, ray* scatteredRay) override{
        applyTextureData(this, hitstream, SPECULAR);
        double brdf_out;
        double pdf_out;
        Vector3 rayDirection = incident.get_direction();
        bool inside = (dot(incident.get_direction(), hitstream->normal) > 0) ? true:false;
        double refractiveratio = inside ? refractiveIndex:inv_refractive;
        Vector3 currentNormal = inside ? hitstream->normal:-hitstream->normal;
        double cosAngleOfIncidence = dot(incident.get_direction(), currentNormal);
        double sinAngleOfIncidence = sqrt(1-cosAngleOfIncidence*cosAngleOfIncidence);
        double sin2AngleOfRefraction = refractiveratio*refractiveratio * (1-cosAngleOfIncidence*cosAngleOfIncidence);
        hitstream->normal = currentNormal;
        double fresnelFactor = this->fresnelShlick(cosAngleOfIncidence, refractiveIndex);
        double randomnum = random_dbl();
        double cosAngleOfRefraction = sqrt(1.0f - sin2AngleOfRefraction);
        if (sin2AngleOfRefraction > 1.0){
            *scatteredRay = ray(hitstream->point, unit(reflect(rayDirection, -currentNormal)));
            brdf_out = 1.0f/abs(cosAngleOfIncidence);
            pdf_out = 1.0f;
            hitstream->normal = -currentNormal;
            hitstream->brdf = brdf_out;
            hitstream->pdf = pdf_out;
            return true;
        }
        if (fresnelFactor > randomnum){
            *scatteredRay = ray(hitstream->point, unit(reflect(rayDirection, -currentNormal)));
            brdf_out = 1.0f/abs(cosAngleOfIncidence);
            pdf_out = 1.0f;
            hitstream->normal = -currentNormal;
            hitstream->brdf = brdf_out;
            hitstream->pdf = pdf_out;
            return true;
        }
        Vector3 jComponent = (rayDirection - currentNormal*cosAngleOfIncidence)/(sinAngleOfIncidence);
        Vector3 refractedDirection = unit(currentNormal*cosAngleOfRefraction +jComponent*sqrt(sin2AngleOfRefraction));
        *scatteredRay = ray(hitstream->point+refractedDirection*0.01, refractedDirection);
        brdf_out = (1.0f - fresnelFactor)/abs(cosAngleOfIncidence);
        pdf_out = 1.0f;
        hitstream->brdf = brdf_out;
        hitstream->pdf = pdf_out;
        return true;
    }

    double BRDF(const ray& incident, const ray& emergent, hitDescriptor* hitstream) override{
        return 1.0f;
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
    Color albedo;
    Color emissive;
    double refractiveIndex;
    double inv_refractive;
    double baseref;
    textureDescriptor *imag;

    double fresnelShlick(double cosineI, double _refractiveIndex) const{
        return baseref + (1-baseref)*((1-cosineI)*(1-cosineI)*(1-cosineI)*(1-cosineI)*(1-cosineI));
    }
};


#endif