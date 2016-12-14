#include "SelectHelper.h"

#include "InputManager.h"

SelectHelper::SelectHelper()
{

}

SelectHelper::~SelectHelper()
{

}

bool SelectHelper::Init(int maxWidth, int maxHeight)
{
	if (maxWidth <= 0 || maxHeight <= 0)
		return false;

	m_maxWidth = maxWidth;
	m_maxHeight = maxHeight;

	m_selectedWidth = 1;
	m_selectedHeight = 1;

	return true;
}

void SelectHelper::UpdateInput(const InputManager* input)
{
	// 入力で選択肢のフォーカスを更新
	if (input->IsFrameKeyDown(UseKeys::Up))
	{
		--m_selectedHeight;
	}
	else if (input->IsFrameKeyDown(UseKeys::Down))
	{
		++m_selectedHeight;
	}
	if (input->IsFrameKeyDown(UseKeys::Left))
	{
		--m_selectedWidth;
	}
	else if (input->IsFrameKeyDown(UseKeys::Right))
	{
		++m_selectedWidth;
	}

	// 選択肢が範囲内になるように補正
	if (m_selectedWidth <= 0)
	{
		m_selectedWidth = m_maxWidth;
	}
	else if (m_selectedWidth > m_maxWidth)
	{
		m_selectedWidth = 1;
	}
	if (m_selectedHeight <= 0)
	{
		m_selectedHeight = m_maxHeight;
	}
	else if (m_selectedHeight > m_maxHeight)
	{
		m_selectedHeight = 1;
	}
}