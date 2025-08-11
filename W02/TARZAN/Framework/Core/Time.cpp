#include "stdafx.h"
#include "Time.h"

Time* Time::_instance = nullptr;

void Time::Init() {
	QueryPerformanceFrequency(&_frequency);
	QueryPerformanceCounter(&_startTime);
}

void Time::_query_frame_update_time() {
	QueryPerformanceCounter(&_frameUpdateTime);
}

void Time::_query_frame_end_time() {
	QueryPerformanceCounter(&_frameEndTime);
	_deltaTime = (_frameEndTime.QuadPart - _frameUpdateTime.QuadPart) * 1.0 / _frequency.QuadPart;
	_elapsedTime = (_frameEndTime.QuadPart - _startTime.QuadPart) * 1.0 / _frequency.QuadPart;
	//OutputDebugString((std::to_wstring(_deltaTime) + L"\n").c_str());
}
