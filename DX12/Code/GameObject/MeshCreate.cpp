#include "MeshCreate.h"
#define PI 3.14159265359
// カプセルメッシュデータを作成 
MeshData CreateCapsule(int divideH, int divideV, float height, float radius)
{
    divideH = divideH < 4 ? 4 : divideH;
    divideV = divideV < 4 ? 4 : divideV;
    radius = radius <= 0 ? 0.001f : radius;

    // 偶数のみ有効  
    if (divideV % 2 != 0)
    {
        divideV++;
    }
  
    int cnt = 0;
    
    // =============================  
    // 頂点座標作成  
    // =============================  
    MeshData CapsuleMesh;

    int vertCount = divideH * divideV + 2;
    CapsuleMesh.vertices.resize(vertCount);

    // 中心角  
    float centerEulerRadianH = (float)((2 * PI) / divideH);
    float centerEulerRadianV = (float)((2 * PI) / divideV);

    float offsetHeight = height * 0.5f;

    // 天面  
    CapsuleMesh.vertices[cnt++] = VECTOR(0, radius + offsetHeight, 0);

    // カプセル上部  
    for (int vv = 0; vv < divideV / 2; vv++)
    {
        float vRadian = (float)(vv + 1) * centerEulerRadianV / 2;

        // 1辺の長さ  
        float tmpLen = (float)sin(vRadian) * radius;
        if (tmpLen < 0)
        {
            tmpLen *= -1;
        }

        float y = (float)cos(vRadian) * radius;
        for (int vh = 0; vh < divideH; vh++)
        {
            // エラーから逃れるよう
            float castNum = (float)vh * centerEulerRadianH;
            VECTOR pos = VECTOR(
                tmpLen * (float)sin(castNum),
                y + offsetHeight,
                tmpLen * (float)cos(castNum)
            );
            // サイズ反映  
            CapsuleMesh.vertices[cnt++] = pos;
        }
    }

    // カプセル下部  
    int offset = divideV / 2;
    for (int vv = 0; vv < divideV / 2; vv++)
    {
        float yRadian = (float)(vv + offset) * centerEulerRadianV / 2;

        // 1辺の長さ  
        float tmpLen = (float)sin(yRadian) * radius;
        if (tmpLen < 0)
        {
            tmpLen *= -1;
        }

        float y = (float)cos(yRadian) * radius;
        for (int vh = 0; vh < divideH; vh++)
        {
            // エラーから逃れるよう
            float castNum = (float)vh * centerEulerRadianH;
            VECTOR pos = VECTOR(
                tmpLen * (float)sin(castNum),
                y - offsetHeight,  
                tmpLen * (float)cos(castNum)
            );
            // サイズ反映  
            CapsuleMesh.vertices[cnt++] = pos;
        }
    }

    // 底面  
    CapsuleMesh.vertices[cnt] = VECTOR(0, -radius - offsetHeight, 0);

    // =============================  
    // インデックス配列作成  
    // =============================  

    int topAndBottomTriCount = divideH * 2;
    // 側面三角形の数  
    int aspectTriCount = divideH * (divideV - 2 + 1) * 2;
    int num = (topAndBottomTriCount + aspectTriCount) * 3;
    CapsuleMesh.indices.resize(num);

    //天面  
    int offsetIndex = 0;
    cnt = 0;
    for (int i = 0; i < divideH * 3; i++)
    {
        if (i % 3 == 0)
        {
            CapsuleMesh.indices[cnt++] = 0;
        }
        else if (i % 3 == 1)
        {
            CapsuleMesh.indices[cnt++] = 1 + offsetIndex;
        }
        else if (i % 3 == 2)
        {
            int index = 2 + offsetIndex++;
            // 蓋をする  
            index = index > divideH ? CapsuleMesh.indices[1] : index;
            CapsuleMesh.indices[cnt++] = index;
        }
    }

    // 側面Index  

    // 頂点を繋ぐイメージ
    // 1 - 2
    // |   |
    // 0 - 3
    //
    // 0, 1, 2
    // 0, 2, 3


     // 開始Index番号  
    int startIndex = CapsuleMesh.indices[1];

    // 天面、底面を除いたカプセルIndex要素数  
    int sideIndexLen = aspectTriCount * 3;

    int lap1stIndex = 0;

    int lap2ndIndex = 0;

    // 一周したときのindex数  
    int lapDiv = divideH * 2 * 3;

    int createSquareFaceCount = 0;

    for (int i = 0; i < sideIndexLen; i++)
    {
        // 一周の頂点数を超えたら更新(初回も含む)  
        if (i % lapDiv == 0)
        {
            lap1stIndex = startIndex;
            lap2ndIndex = startIndex + divideH;
            createSquareFaceCount++;
        }

        if (i % 6 == 0 || i % 6 == 3)
        {
            CapsuleMesh.indices[cnt++] = startIndex;
        }
        else if (i % 6 == 1)
        {
            CapsuleMesh.indices[cnt++] = startIndex + divideH;
        }
        else if (i % 6 == 2 || i % 6 == 4)
        {
            if (i > 0 &&
                (i % (lapDiv * createSquareFaceCount - 2) == 0 ||
                    i % (lapDiv * createSquareFaceCount - 4) == 0)
                )
            {
                // 1周したときのClamp処理  
                // 周回ポリゴンの最後から2番目のIndex  
                CapsuleMesh.indices[cnt++] = lap2ndIndex;
            }
            else
            {
                CapsuleMesh.indices[cnt++] = startIndex + divideH + 1;
            }
        }
        else if (i % 6 == 5)
        {
            if (i > 0 && i % (lapDiv * createSquareFaceCount - 1) == 0)
            {
                // 1周したときのClamp処理  
                // 周回ポリゴンの最後のIndex  
                CapsuleMesh.indices[cnt++] = lap1stIndex;
            }
            else
            {
                CapsuleMesh.indices[cnt++] = startIndex + 1;
            }

            // 開始Indexの更新  
            startIndex++;
        }
    }


    // 底面Index  
    offsetIndex = (int)CapsuleMesh.vertices.size() - 1 - divideH;
    lap1stIndex = offsetIndex;
    int finalIndex = (int)CapsuleMesh.vertices.size() - 1;
    int len = divideH * 3;

    for (int i = len - 1; i >= 0; i--)
    {
        if (i % 3 == 0)
        {
            // 底面の先頂点  
            CapsuleMesh.indices[cnt++] = finalIndex;
            offsetIndex++;
        }
        else if (i % 3 == 1)
        {
            CapsuleMesh.indices[cnt++] = offsetIndex;
        }
        else if (i % 3 == 2)
        {
            int value = 1 + offsetIndex;
            if (value >= CapsuleMesh.vertices.size() - 1)
            {
                value = lap1stIndex;
            }

            CapsuleMesh.indices[cnt++] = value;
        }
    }


    return CapsuleMesh;
}