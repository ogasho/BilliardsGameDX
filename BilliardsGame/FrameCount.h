/* �t���[���J�E���g�N���X */
// �o�߃t���[�����J�E���g����

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

