#include "MeshCreate.h"
#define PI 3.14159265359
// �J�v�Z�����b�V���f�[�^���쐬 
MeshData CreateCapsule(int divideH, int divideV, float height, float radius)
{
    divideH = divideH < 4 ? 4 : divideH;
    divideV = divideV < 4 ? 4 : divideV;
    radius = radius <= 0 ? 0.001f : radius;

    // �����̂ݗL��  
    if (divideV % 2 != 0)
    {
        divideV++;
    }
  
    int cnt = 0;
    
    // =============================  
    // ���_���W�쐬  
    // =============================  
    MeshData CapsuleMesh;

    int vertCount = divideH * divideV + 2;
    CapsuleMesh.vertices.resize(vertCount);

    // ���S�p  
    float centerEulerRadianH = (float)((2 * PI) / divideH);
    float centerEulerRadianV = (float)((2 * PI) / divideV);

    float offsetHeight = height * 0.5f;

    // �V��  
    CapsuleMesh.vertices[cnt++] = VECTOR(0, radius + offsetHeight, 0);

    // �J�v�Z���㕔  
    for (int vv = 0; vv < divideV / 2; vv++)
    {
        float vRadian = (float)(vv + 1) * centerEulerRadianV / 2;

        // 1�ӂ̒���  
        float tmpLen = (float)sin(vRadian) * radius;
        if (tmpLen < 0)
        {
            tmpLen *= -1;
        }

        float y = (float)cos(vRadian) * radius;
        for (int vh = 0; vh < divideH; vh++)
        {
            // �G���[���瓦���悤
            float castNum = (float)vh * centerEulerRadianH;
            VECTOR pos = VECTOR(
                tmpLen * (float)sin(castNum),
                y + offsetHeight,
                tmpLen * (float)cos(castNum)
            );
            // �T�C�Y���f  
            CapsuleMesh.vertices[cnt++] = pos;
        }
    }

    // �J�v�Z������  
    int offset = divideV / 2;
    for (int vv = 0; vv < divideV / 2; vv++)
    {
        float yRadian = (float)(vv + offset) * centerEulerRadianV / 2;

        // 1�ӂ̒���  
        float tmpLen = (float)sin(yRadian) * radius;
        if (tmpLen < 0)
        {
            tmpLen *= -1;
        }

        float y = (float)cos(yRadian) * radius;
        for (int vh = 0; vh < divideH; vh++)
        {
            // �G���[���瓦���悤
            float castNum = (float)vh * centerEulerRadianH;
            VECTOR pos = VECTOR(
                tmpLen * (float)sin(castNum),
                y - offsetHeight,  
                tmpLen * (float)cos(castNum)
            );
            // �T�C�Y���f  
            CapsuleMesh.vertices[cnt++] = pos;
        }
    }

    // ���  
    CapsuleMesh.vertices[cnt] = VECTOR(0, -radius - offsetHeight, 0);

    // =============================  
    // �C���f�b�N�X�z��쐬  
    // =============================  

    int topAndBottomTriCount = divideH * 2;
    // ���ʎO�p�`�̐�  
    int aspectTriCount = divideH * (divideV - 2 + 1) * 2;
    int num = (topAndBottomTriCount + aspectTriCount) * 3;
    CapsuleMesh.indices.resize(num);

    //�V��  
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
            // �W������  
            index = index > divideH ? CapsuleMesh.indices[1] : index;
            CapsuleMesh.indices[cnt++] = index;
        }
    }

    // ����Index  

    // ���_���q���C���[�W
    // 1 - 2
    // |   |
    // 0 - 3
    //
    // 0, 1, 2
    // 0, 2, 3


     // �J�nIndex�ԍ�  
    int startIndex = CapsuleMesh.indices[1];

    // �V�ʁA��ʂ��������J�v�Z��Index�v�f��  
    int sideIndexLen = aspectTriCount * 3;

    int lap1stIndex = 0;

    int lap2ndIndex = 0;

    // ��������Ƃ���index��  
    int lapDiv = divideH * 2 * 3;

    int createSquareFaceCount = 0;

    for (int i = 0; i < sideIndexLen; i++)
    {
        // ����̒��_���𒴂�����X�V(������܂�)  
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
                // 1�������Ƃ���Clamp����  
                // ����|���S���̍Ōォ��2�Ԗڂ�Index  
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
                // 1�������Ƃ���Clamp����  
                // ����|���S���̍Ō��Index  
                CapsuleMesh.indices[cnt++] = lap1stIndex;
            }
            else
            {
                CapsuleMesh.indices[cnt++] = startIndex + 1;
            }

            // �J�nIndex�̍X�V  
            startIndex++;
        }
    }


    // ���Index  
    offsetIndex = (int)CapsuleMesh.vertices.size() - 1 - divideH;
    lap1stIndex = offsetIndex;
    int finalIndex = (int)CapsuleMesh.vertices.size() - 1;
    int len = divideH * 3;

    for (int i = len - 1; i >= 0; i--)
    {
        if (i % 3 == 0)
        {
            // ��ʂ̐撸�_  
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