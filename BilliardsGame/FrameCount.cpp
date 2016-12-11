#include "FrameCount.h"


FrameCount::FrameCount()
{
	Reset();
}

FrameCount::~FrameCount()
{

}

void FrameCount::Reset()
{
	m_countFrame = 0;
}

void FrameCount::CountingFrame()
{
	++m_countFrame;
}