#pragma once
#include "../VECTOR.h"
#include <vector>
struct MeshData
{
    std::vector<VECTOR> vertices;
    std::vector<int> indices;
};

// カプセルメッシュデータを作成 
MeshData CreateCapsule(int divideH, int divideV, float height = 1, float radius = 0.5f);