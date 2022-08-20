#include "PointLineComponent.h"
#include "Actor.h"
#include "../GameObject/PointLine.h"

PointLineComponent::PointLineComponent(Actor* actor, int drawOrder)
	:ObjectComponent(actor, drawOrder)
	, mPointData()
	, mLineData()
{

}
PointLineComponent::~PointLineComponent()
{
	for (PointData* point : mPointData)
	{
		delete point;
	}

	for (LineData* line : mLineData)
	{
		delete line;
	}
}
void PointLineComponent::SetPoint()
{
	for (PointData* point : mPointData)
	{
		PointLine::GetInstance()->SetPointMetaData(point->entryPos, point->pos, point->color);
	}
}

void PointLineComponent::DeletePoint()
{
	for (PointData* point : mPointData)
	{
		PointLine::GetInstance()->PointDeleate(point->entryPos);
	}
}
void PointLineComponent::SetLine()
{
	for (LineData* line : mLineData)
	{
		PointLine::GetInstance()->SetLineMetaData(line->entryPos, line->startPos, line->endPos, line->color);
	}
}

void PointLineComponent::DeleteLine()
{
	for (LineData* line : mLineData)
	{
		PointLine::GetInstance()->LineDeleate(line->entryPos);
	}
}