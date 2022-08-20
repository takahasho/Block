#pragma once
#include "ObjectComponent.h"

// ì_èÓïÒ
struct PointData
{
    VECTOR pos;
    VECTOR color;
    int entryPos;
    PointData()
    {
        pos = VECTOR(0, 0, 0);
        color = VECTOR(0, 0, 0);
        entryPos = -1;
    }
};
// ê¸èÓïÒ
struct LineData
{
    VECTOR startPos;
    VECTOR endPos;
    VECTOR color;
    int entryPos;
    LineData()
    {
        startPos = VECTOR(0, 0, 0);
        endPos = VECTOR(0, 0, 0);
        color = VECTOR(0, 0, 0);
        entryPos = -1;
    }
};
class PointLineComponent :
    public ObjectComponent
{

public:
    PointLineComponent(class Actor* actor, int drawOrder = 100);
    ~PointLineComponent();
    void SetPoint();
    void DeletePoint();
    void SetLine();
    void DeleteLine();
    PointData* GetPointData(int num) { return mPointData[num]; }
    LineData* GetLineData(int num) { return mLineData[num]; }
    void resizeLineData(int resizeNum)
    {
        DeleteLine();
        for (int i = resizeNum, size = (int)mLineData.size(); i < size; ++i)
        {
            delete mLineData[i];
        }
        mLineData.resize(resizeNum);
    }

    void SetLineData(int num, const LineData data) {
        if (mLineData[num] == nullptr)mLineData[num] = new LineData;
        mLineData[num]->startPos = data.startPos;
        mLineData[num]->endPos = data.endPos;
        mLineData[num]->color = data.color;
    }
private:
    std::vector<PointData*> mPointData;
    std::vector<LineData*> mLineData;
};

