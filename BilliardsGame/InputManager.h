/* ���͏����N���X */
// ���͏�Ԃ�ۊǁA�񋟂���
// ���͏�Ԃ̓E�B���h�E�v���V�[�W���ōX�V����

#pragma once

enum class UseKeys
{
	Up,
	Down,
	Left,
	Right,
	Enter,
	Space,
	Esc,
};

class InputManager
{
public:
	InputManager();
	~InputManager();

	void Init();

	void KeyDown(unsigned int input);
	void KeyUp(unsigned int input);
	void UpdateKeyState(); // �t���[���X�V�̍ۂɌĂ�

	bool IsKeyDown(unsigned int key)const;
	bool IsKeyDown(UseKeys key)const;
	bool IsFrameKeyDown(unsigned int key)const;
	bool IsFrameKeyDown(UseKeys key)const;

private:
	unsigned int TransUseKeys(UseKeys key)const;

	bool m_keys[256];
	bool m_oldKeys[256];
};

