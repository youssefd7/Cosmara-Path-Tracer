#ifndef modelreader_h
#define modelreader_h

#include <filesystem>

#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>

struct Vertex {
    float vertex1 = 0;
    float vertex2 = 0;
    float vertex3 = 0;
    unsigned long pos;
};

class ModelReader{
    public:
    Vertex baseVertex;
    const char* filepath;
    Vertex* verticiesArray;
    long triangleCount;
    long globalCount = 0;
    float increment;
    std::vector<Vertex> intermediateArr;

    ModelReader(){

    }

    ModelReader(const char* path):filepath(path){
        triangleCount = this->retrieveVertexCount(false);
    }

    void initReader(const char* path){
        filepath = path;
        triangleCount = this->retrieveVertexCount(false);
        intermediateArr.reserve(sizeof(Vertex)*triangleCount*3);
    }

    void initArray(Vertex* vertexBuffer){
        verticiesArray = vertexBuffer;
    }

    void processLine(std::vector<Vertex>* vertexArray, std::string currentLine, long count){
        std::string currentNumber = "";
        std::vector<float> verticies;
        if (currentLine[0] == 'f'){
            for (short i = 2; i < currentLine.size(); ++i){
                if ((char)currentLine[i] == 32){
                    this->verticiesArray[globalCount] = intermediateArr[std::stoi(currentNumber)-1];
                    currentNumber = "";
                    globalCount++;
                }
                else {
                    currentNumber += currentLine[i];
                }
            }
            this->verticiesArray[globalCount] = intermediateArr[std::stoi(currentNumber)-1];
            globalCount++;
            return;
        }
        else if (currentLine[0] == 'v'){
            for (short i = 2; i < currentLine.size(); ++i){
                if ((char)currentLine[i] == 32){
                    verticies.push_back(std::stof(currentNumber));
                    currentNumber = "";
                }
                else {
                    currentNumber += currentLine[i];
                }
            }
            verticies.push_back(std::stof(currentNumber));
            Vertex processedVertex;
            processedVertex.vertex1 = verticies[0];
            processedVertex.vertex2 = verticies[1];
            processedVertex.vertex3 = verticies[2];
            processedVertex.pos = count-globalCount;
            vertexArray->push_back(processedVertex);
        }
    }
    
    void printData(std::vector<Vertex>* vertexArray, std::string filename){
        std::cout << "Finished Processing!\n";
        std::cout << filename << " has a total of " << this->retrieveVertexCount() << " base verticies\n";
        std::cout << filename << " has a total of " << this->triangleCount << " triangles\n";
    }
    
    void ReadObj(){
        long count = 0;
        std::ifstream model(filepath);
        std::string currentCoordinate;
        while (std::getline(model, currentCoordinate)){
            processLine(&intermediateArr, currentCoordinate, count);
            count++;
        }
        printData(&intermediateArr, filepath);
    }
    
    long retrieveVertexCount(bool mode = true){
        long count = 0;
        std::ifstream model(filepath);
        std::string currentCoordinate;
        while (std::getline(model, currentCoordinate)){
            if (mode){
                if (currentCoordinate[0] == 'v'){count++;}
            }
            else {
                if (currentCoordinate[0] == 'f'){count++;}
            }
        }
        return count;
    }
};

#endif 