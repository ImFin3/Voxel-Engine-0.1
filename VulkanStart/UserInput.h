#ifndef USER_INPUT_H
#define USER_INPUT_H

enum class InputState
{
	NEGATIVE = -1,
	NEUTRAL = 0,
	POSITIVE = 1
};

class UserInput {
public:
	InputState lengthways;
	InputState sideways;
	InputState vertical;

	float xPosDelta, yPosDelta;

	void SetNull();

	void Forward();

	void Backward();

	void Left();

	void Right();

	void Up();

	void Down();

	void GetMouseInputDelta(const double a_xNew, const double a_yNew, double* a_xOld, double* a_yOld);
	
};

#endif // !USER_INPUT_H

