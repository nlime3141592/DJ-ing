#include "LoopMessage.h"

#define DJSW_LOOP_MESSAGE_CAPACITY 128

static djMessage _msgQueue[DJSW_LOOP_MESSAGE_CAPACITY];
static int _msgBeg;
static int _msgEnd;

djErrorCode PeekMessage(djMessage* msg)
{
	if (_msgBeg == _msgEnd)
		return DJSW_ERR_ERROR;

	int nextBeg = (_msgBeg + 1) % DJSW_LOOP_MESSAGE_CAPACITY;

	*msg = _msgQueue[nextBeg];

	return DJSW_ERR_NO_ERROR;
}

djErrorCode GetMessage(djMessage* msg)
{
	if (_msgBeg == _msgEnd)
		return DJSW_ERR_ERROR;

	int nextBeg = (_msgBeg + 1) % DJSW_LOOP_MESSAGE_CAPACITY;

	_msgBeg = nextBeg;
	*msg = _msgQueue[nextBeg];

	return DJSW_ERR_NO_ERROR;
}

djErrorCode AddMessage(djMessage* msg)
{
	int nextEnd = (_msgEnd + 1) % DJSW_LOOP_MESSAGE_CAPACITY;

	if (nextEnd == _msgBeg)
		return DJSW_ERR_ERROR;

	_msgEnd = nextEnd;
	_msgQueue[nextEnd] = *msg;

	return DJSW_ERR_NO_ERROR;
}

djErrorCode MessageInit()
{
	_msgBeg = 0;
	_msgEnd = 0;

	return DJSW_ERR_NO_ERROR;
}

djErrorCode MessageUpdate()
{
	djMessage msg;

	while (_msgBeg != _msgEnd)
	{
		if (GetMessage(&msg) == DJSW_ERR_ERROR)
			continue;

		// TODO: handling message here.
	}

	return DJSW_ERR_NO_ERROR;
}

djErrorCode MessageFinal()
{
	return DJSW_ERR_NO_ERROR;
}