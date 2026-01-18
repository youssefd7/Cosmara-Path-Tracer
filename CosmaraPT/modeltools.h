#ifndef modeltools_h
#define modeltools_h

#include "modelreader.h"

class modeltools {
    public:
    const char* filepath;
    Vertex* vertexBuffer;

    modeltools(ModelReader* reader):model(reader){
        reader->ReadObj();
        vertexBuffer = reader->verticiesArray;
    }

    ~modeltools(){}

    void applyRoll(double angle){
        double radangle = rad(angle);
        for (unsigned long i = 0; i < model->triangleCount*3; ++i){
            model->verticiesArray[i].vertex2 = vertexBuffer[i].vertex2 * cos(radangle) - vertexBuffer[i].vertex3 * sin(radangle);
            model->verticiesArray[i].vertex3 = vertexBuffer[i].vertex2 * sin(radangle) + vertexBuffer[i].vertex3 * cos(radangle);

            model->intermediateArr[i].vertex2 = vertexBuffer[i].vertex2 * cos(radangle) - vertexBuffer[i].vertex3 * sin(radangle);
            model->intermediateArr[i].vertex3 = vertexBuffer[i].vertex2 * sin(radangle) + vertexBuffer[i].vertex3 * cos(radangle);
        }

    }

    void applyPitch(double angle){
        double radangle = rad(angle);
        for (unsigned long i = 0; i < model->triangleCount*3; ++i){
            model->verticiesArray[i].vertex1 = vertexBuffer[i].vertex1 * cos(radangle) + vertexBuffer[i].vertex3 * sin(radangle);
            model->verticiesArray[i].vertex3 = -vertexBuffer[i].vertex1 * sin(radangle) + vertexBuffer[i].vertex3 * cos(radangle);

            model->intermediateArr[i].vertex1 = vertexBuffer[i].vertex1 * cos(radangle) + vertexBuffer[i].vertex3 * sin(radangle);
            model->intermediateArr[i].vertex3 = -vertexBuffer[i].vertex1 * sin(radangle) + vertexBuffer[i].vertex3 * cos(radangle);
        }
    }

    void applyYaw(double angle){
        double radangle = rad(angle);
        for (unsigned long i = 0; i < model->triangleCount*3; ++i){
            model->verticiesArray[i].vertex1 = vertexBuffer[i].vertex1 * cos(radangle) - vertexBuffer[i].vertex2 * sin(radangle);
            model->verticiesArray[i].vertex2 = vertexBuffer[i].vertex1 * sin(radangle) + vertexBuffer[i].vertex2 * cos(radangle);

            model->intermediateArr[i].vertex1 = vertexBuffer[i].vertex1 * cos(radangle) - vertexBuffer[i].vertex2 * sin(radangle);
            model->intermediateArr[i].vertex2 = vertexBuffer[i].vertex1 * sin(radangle) + vertexBuffer[i].vertex2 * cos(radangle);
        }
    }

    void scale(double scalefactor){
        for (unsigned long i = 0; i < model->triangleCount*3; ++i){
            model->verticiesArray[i].vertex1 = vertexBuffer[i].vertex1 * scalefactor;
            model->verticiesArray[i].vertex2 = vertexBuffer[i].vertex2 * scalefactor;
            model->verticiesArray[i].vertex3 = vertexBuffer[i].vertex3 * scalefactor;

            model->intermediateArr[i].vertex1 = vertexBuffer[i].vertex1 * scalefactor;
            model->intermediateArr[i].vertex2 = vertexBuffer[i].vertex2 * scalefactor;
            model->intermediateArr[i].vertex3 = vertexBuffer[i].vertex3 * scalefactor;
        }
    }

    void translate(Vector3 translation){
        double x = translation.x();
        double y = translation.y();
        double z = translation.z();
        for (unsigned long i = 0; i < model->triangleCount*3; ++i){
            model->verticiesArray[i].vertex1 = vertexBuffer[i].vertex1 + x;
            model->verticiesArray[i].vertex2 = vertexBuffer[i].vertex2 + y;
            model->verticiesArray[i].vertex3 = vertexBuffer[i].vertex3 + z;
            model->intermediateArr[i].vertex1 = vertexBuffer[i].vertex1 + x;
            model->intermediateArr[i].vertex2 = vertexBuffer[i].vertex2 + y;
            model->intermediateArr[i].vertex3 = vertexBuffer[i].vertex3 + z;
        }
    }

    ModelReader* retrieveReader(){
        return model;
    }

    private:
    ModelReader* model;
};

#endif