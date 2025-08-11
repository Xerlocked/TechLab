#pragma once
#include "Windows.h"
class Time {
private:
	double _deltaTime;
	double _elapsedTime;
	LARGE_INTEGER _frequency, _startTime, _frameUpdateTime, _frameEndTime;
private:
	static Time* _instance;
	Time(): _deltaTime(0.f), _elapsedTime(0.f) {};
	Time(Time& ref) {};
	Time& operator=(Time& ref) {};
	~Time() {};
public:
	static Time* Instance() {
		if ( _instance == nullptr )
			_instance = new Time();
		return _instance;
	}
public:
	void Init();
	static double GetDeltaTime() { return _instance->_deltaTime; }
	static double GetElapsedTime() { return _instance->_elapsedTime; }
	void _query_frame_update_time();
	void _query_frame_end_time();
};

