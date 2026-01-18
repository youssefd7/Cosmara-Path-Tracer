#ifndef GLOSSY_H
#define GLOSSY_H

#pragma once

#include "material.h"
#include "utils.h"

class glossy : public material {
    public:
    MATERIALTYPE mat_name = GLOSSY;

    glossy(Color _albedo, Color _emissive, double _roughness, double _reflectivity):albedo(_albedo), emissive(_emissive), roughness(_roughness), reflectivity(_reflectivity){
        alpha = roughness*roughness;
        diffuse = ((albedo.x()+albedo.y()+albedo.z())/3.0f)*(1.0f/PI);
        baseref = pow(((1-reflectivity)/(1+reflectivity)), 2);
    }
    glossy(Color _albedo, Color _emissive, double _roughness, double _reflectivity,textureDescriptor* _imag):albedo(_albedo), emissive(_emissive),roughness(_roughness), reflectivity(_reflectivity),imag(_imag){
        alpha = roughness*roughness;
        diffuse = ((albedo.x()+albedo.y()+albedo.z())/3.0f)*(1.0f/PI);
    }

    bool generateRayAndAttenuation(const ray& incident, hitDescriptor* hitstream, ray* scatteredRay) override{
        applyTextureData(this,hitstream, DIFFUSE);
        applyTextureData(this,hitstream, SPECULAR);
        double pdf_val;
        Vector3 scatteredDirection;
        Vector3 microfacetNormal = hitstream->normal;

        DISTRIBUTION diffuseDistribution = COSINE;
        DISTRIBUTION importancesample = BECKMANN;

        if (random_dbl() > roughness){
            scatteredDirection = importancesample == BECKMANN ? unit(sampleBeckmann(hitstream, &microfacetNormal)) : unit(generateDirInHemisphere(hitstream->normal, diffuseDistribution));
            *scatteredRay = ray(hitstream->point, scatteredDirection);
        }
        else {
            scatteredDirection = unit(generateDirInHemisphere(hitstream->normal, diffuseDistribution));
            *scatteredRay = ray(hitstream->point, scatteredDirection);
        }

        if (importancesample == BECKMANN){
            double pdfSpecular = PDFSpecular(incident, *scatteredRay, hitstream, microfacetNormal);
            double pdfDiffuse = PDFDiffuse(incident, *scatteredRay, hitstream, microfacetNormal);

            double specularWeight = power_heuristic(pdfSpecular, pdfDiffuse);
            double diffuseWeight = power_heuristic(pdfDiffuse, pdfSpecular);

            hitstream->brdf = clamp(0.0, 1.0, specularWeight * BRDFSpecular(incident, *scatteredRay, hitstream, microfacetNormal) + diffuseWeight * BRDFDiffuse(incident, *scatteredRay, hitstream, microfacetNormal));
            hitstream->pdf = clamp(0.0, 1.0, specularWeight*pdfSpecular + diffuseWeight*pdfDiffuse);
        }
        else {
            hitstream->brdf = BRDFSpecular(incident, *scatteredRay, hitstream, microfacetNormal) + BRDFDiffuse(incident, *scatteredRay, hitstream, microfacetNormal);
            hitstream->pdf = dot(scatteredRay->get_direction(), hitstream->normal)/PI;
        }
        return true;
    }

    double BRDFSpecular(const ray& incident, const ray& emergent, hitDescriptor* hitstream, Vector3& microfacetNormal) {
        Vector3 rayDir = -incident.get_direction();
        Vector3 emergentDir = emergent.get_direction();
        Vector3 halfVector = unit(emergentDir+rayDir);

        double cosIncidence = std::max(0.001, dot(rayDir, hitstream->normal));
        double cosEmergence = std::max(0.001, dot(emergentDir, hitstream->normal));

        double fresnel = fresnelShlick(dot(halfVector, rayDir));
        double geometricAttenuation = SmithGGX(rayDir, hitstream, microfacetNormal)*SmithGGX(emergentDir, hitstream, microfacetNormal);
        double BeckmannValue = BeckmannDistribution(halfVector, hitstream, microfacetNormal);
        double specular = (fresnel*geometricAttenuation*BeckmannValue)/(4*cosIncidence*cosEmergence);
        return specular;
    };

    double BRDFDiffuse(const ray& incident, const ray& emergent, hitDescriptor* hitstream, Vector3& microfacetNormal){
        return diffuse;
    }

    double PDFSpecular(const ray& incident, const ray& emergent, hitDescriptor* hitstream, Vector3& microfacetNormal) {
        Vector3 rayDir = -incident.get_direction();
        Vector3 emergentDir = emergent.get_direction();
        Vector3 halfVector = unit(emergentDir+rayDir);
        double pdf_val = BeckmannPDF(halfVector, hitstream, microfacetNormal);
        return pdf_val;
    };

    double PDFDiffuse(const ray& incident, const ray& emergent, hitDescriptor* hitstream, Vector3& microfacetNormal){
        return dot(emergent.get_direction(), hitstream->normal)/PI;
    }

    double SmithGGX(Vector3& inputVec, hitDescriptor* hitstream, Vector3& microfacetNormal){
        double dotVal = std::max(0.001, dot(inputVec, hitstream->normal));
        double alpha2 = alpha*alpha;
        return 2.0f * dotVal/(dotVal + sqrt(alpha2 + (1-alpha2)*dotVal*dotVal));
    }

    double BeckmannDistribution(Vector3& halfVec, hitDescriptor* hitstream, Vector3& microfacetNormal){
        double cosineHalf = std::max(0.001, dot(halfVec, hitstream->normal));
        double tan_theta = sqrt(1-cosineHalf*cosineHalf)/cosineHalf;
        double expterm = exp(-(tan_theta*tan_theta)/(alpha*alpha));
        return expterm/(PI*alpha*alpha*cosineHalf*cosineHalf*cosineHalf*cosineHalf);
    }

    double BeckmannPDF(Vector3& halfVec, hitDescriptor* hitstream, Vector3& microfacetNormal){
        double dotVal = std::max(0.001, dot(halfVec, hitstream->normal));
        return BeckmannDistribution(halfVec, hitstream, microfacetNormal)*dotVal;
    }

    double fresnelShlick(double cosineI) const{
        return baseref + (1-baseref)*((1-cosineI)*(1-cosineI)*(1-cosineI)*(1-cosineI)*(1-cosineI));
    }

    double power_heuristic(double pdf1, double pdf2){
        return pdf1/(pdf1+pdf2+1e-6);
    }

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
    double roughness;
    double reflectivity;
    double alpha;
    double diffuse;
    double baseref;
    textureDescriptor* imag;

    Vector3 sampleBeckmann(hitDescriptor* hitstream, Vector3* normalOut){
        double r1 = random_dbl();
        double r2 = random_dbl();

        double theta = std::atan(sqrt(-alpha*alpha*std::log(1-r1)));
        double phi = 2*PI*r2;

        Vector3 sampledNormal(cos(phi)*sin(theta), cos(theta), sin(phi)*sin(theta));
        Vector3 up = hitstream->normal == Vector3(0,1,0) ? Vector3(1,0,0): Vector3(0,1,0);
        Vector3 tangent = unit(cross(hitstream->normal, up));
        Vector3 bitangent = unit(cross(hitstream->normal, tangent));
        Vector3 sampledDirection = sampledNormal.x()*tangent + sampledNormal.y()*hitstream->normal + sampledNormal.z()*bitangent;
        *normalOut = sampledNormal;
        return sampledDirection;
    }
};

#endif