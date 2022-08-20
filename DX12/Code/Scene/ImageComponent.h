#pragma once
#include "Component.h"
#include "../VECTOR.h"
#include "../VECTOR2.h"
#include <vector>
#include <string>

// 画像情報
struct ImageData
{
    const char* filename;
    float width, height;
    int entryPos;
    ImageData(const char* name)
    {
        width = height = 0;
        filename = name;
        entryPos = -1;
    }
};

class ImageComponent :
    public Component
{
public:
    ImageComponent(class Actor*, int drawOrder = 0);  // コンストラクタ
    ~ImageComponent();             //　デストラクタ

    virtual void Update();
    virtual void Draw();

    void SetFileName(const char* fileName);
    void DeleateImage();

    int GetDrawOrder() const { return mDrawOrder; }
    void SetDrawOrder(int value) { mDrawOrder = value; }
    ImageData* GetImageData() { return mImageData; }
private:
    int mDrawOrder;
    ImageData* mImageData;
};