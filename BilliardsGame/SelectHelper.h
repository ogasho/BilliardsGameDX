/* セレクトヘルパークラス */
// 選択肢を管理するためのクラス

#pragma once

class InputManager;

class SelectHelper
{
public:
	SelectHelper();
	~SelectHelper();

	bool Init(int maxWidth, int maxHeight);
	void UpdateInput(const InputManager* input);

	int GetMaxWidth()const{ return m_maxWidth; }
	int GetMaxHeight()const{ return m_maxHeight; }
	int GetSelectedWidth()const{ return m_selectedWidth; }
	int GetSelectedHeight()const{ return m_selectedHeight; }

private:
	int m_maxWidth;
	int m_maxHeight;

	int m_selectedWidth;
	int m_selectedHeight;
};

