/* フレームカウントクラス */
// 経過フレームをカウントする

#pragma once
class FrameCount
{
public:
	FrameCount();
	~FrameCount();

	void Reset();
	void CountingFrame();

	int GetCountFrame()const{ return m_countFrame; }

private:
	int m_countFrame;
};

