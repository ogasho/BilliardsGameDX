/* ���͏����N���X */
// ���͏�Ԃ�ۊǁA�񋟂���
// ���͏�Ԃ̓E�B���h�E�v���V�[�W���ōX�V����

#pragma once

class InputManager
{
public:
	InputManager();
	~InputManager();

	void Init();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];

};

