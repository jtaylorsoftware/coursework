/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

// simple class that keeps track of time to be used in calculating the delta time between frames
class Timer
{
public:
	Timer(double(*timeFunc)(void)) : currentTime(0), previousTime(0), deltaTime(0), timeFunc(timeFunc){}
	~Timer(){}
	void Tick(){
		currentTime = static_cast<float>(timeFunc());
		deltaTime = currentTime - previousTime;
		previousTime = currentTime;
	}
	float GetCurrentTime(){ return currentTime; }
	float GetPreviousTime(){ return previousTime; }
	float GetDeltaTime(){ return deltaTime; }
private:
	float currentTime, previousTime, deltaTime;
	double(*timeFunc)(void);
};