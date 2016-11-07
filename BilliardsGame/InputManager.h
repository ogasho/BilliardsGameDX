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

	bool IsKeyDown(unsigned int key);
	bool IsKeyDown(UseKeys key);
	bool IsFrameKeyDown(unsigned int key);
	bool IsFrameKeyDown(UseKeys key);

private:
	unsigned int TransUseKeys(UseKeys key);

	bool m_keys[256];
	bool m_oldKeys[256];
};

