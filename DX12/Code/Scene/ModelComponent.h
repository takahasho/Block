#pragma once
#include "ObjectComponent.h"
#include "../VECTOR.h"
class ModelComponent :
    public ObjectComponent
{
public:
    ModelComponent(class Actor* owner, int drawOrder = 100);
    void SetObjModel();
    void DeleteObjModel();
    void SetFileName(const char* fileName) { mFileName = fileName; }
    const char* GetFileName() const { return mFileName; }
    std::vector<VECTOR> GetModelVertex();
private:
   const char* mFileName;
};

