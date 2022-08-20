#pragma once
#include "Component.h"
#include "../GameObject/ObjectStruct.h"

// ---------------------- クラス宣言 ---------------------
class ObjectComponent:
    public Component
{
protected:
    // ---------------------- 構造体宣言 ---------------------
    // Object情報
    struct ObjectData
    {
        VECTOR position;
        VECTOR rotation;
        VECTOR scale;
        VECTOR color;
        int entryPos;
        ObjectData()
        {
            position = VECTOR(0, 0, 0);
            rotation = VECTOR(0, 0, 0);
            scale = VECTOR(1, 1, 1);
            color = VECTOR(1, 1, 1);
            entryPos = -1;
        }
    };
    ObjectType mObjectType;
    ObjectData mObjectData;
public:
    ObjectComponent(class Actor*, int drawOrder = 100);  // コンストラクタ
    ~ObjectComponent();         // デストラクタ

    void SetObject();
    void DeleteObject();
    int GetDrawOrder() const { return mDrawOrder; }
    void SetObjectType(ObjectType type) { mObjectType = type; }
    ObjectType GetObjectType() const { return mObjectType; }
private:
    int mDrawOrder;
};
