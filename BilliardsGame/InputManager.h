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

	void KeyDown(unsigned int input);
	void KeyUp(unsigned int input);

	bool IsKeyDown(unsigned int key);

private:
	bool m_keys[256];

};

