#include "UserInput.h"

void UserInput::SetNull()
{
	lengthways = InputState::NEUTRAL;
	sideways = InputState::NEUTRAL;
	vertical = InputState::NEUTRAL;
}

void UserInput::Forward()
{
	switch (lengthways)
	{
	case InputState::NEGATIVE:
		lengthways = InputState::NEUTRAL;
		break;
	case InputState::NEUTRAL:
		lengthways = InputState::POSITIVE;
		break;
	case InputState::POSITIVE:
		break;
	default:
		break;
	}
}

void UserInput::Backward()
{
	switch (lengthways)
	{
	case InputState::NEGATIVE:
		break;
	case InputState::NEUTRAL:
		lengthways = InputState::NEGATIVE;
		break;
	case InputState::POSITIVE:
		lengthways = InputState::NEUTRAL;
		break;
	default:
		break;
	}
}

void UserInput::Left()
{
	switch (sideways)
	{
	case InputState::NEGATIVE:
		sideways = InputState::NEUTRAL;
		break;
	case InputState::NEUTRAL:
		sideways = InputState::POSITIVE;
		break;
	case InputState::POSITIVE:
		break;
	default:
		break;
	}
}

void UserInput::Right()
{
	switch (sideways)
	{
	case InputState::NEGATIVE:
		break;
	case InputState::NEUTRAL:
		sideways = InputState::NEGATIVE;
		break;
	case InputState::POSITIVE:
		sideways = InputState::NEUTRAL;
		break;
	default:
		break;
	}
}

void UserInput::Up()
{
	switch (vertical)
	{
	case InputState::NEGATIVE:
		vertical = InputState::NEUTRAL;
		break;
	case InputState::NEUTRAL:
		vertical = InputState::POSITIVE;
		break;
	case InputState::POSITIVE:
		break;
	default:
		break;
	}
}

void UserInput::Down()
{
	switch (vertical)
	{
	case InputState::NEGATIVE:
		break;
	case InputState::NEUTRAL:
		vertical = InputState::NEGATIVE;
		break;
	case InputState::POSITIVE:
		vertical = InputState::NEUTRAL;
		break;
	default:
		break;
	}
}

void UserInput::GetMouseInputDelta(const double a_xNew, const double a_yNew, double* a_xOld, double* a_yOld)

{
	xPosDelta = a_xNew - *a_xOld;
	yPosDelta = a_yNew - *a_yOld;

	*a_xOld = a_xNew;
	*a_yOld = a_yNew;
}
