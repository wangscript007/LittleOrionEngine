#include "Timer.h"

#include <SDL/SDL.h>

Timer::~Timer()
{
}

void Timer::Start()
{
	start_ticks = SDL_GetTicks();
	running = true;
}

bool Timer::Started()
{
	return running;
}

float Timer::Read() const
{
	if (running)
	{
		if (paused) 
		{
			return pause_time;
		}
		else
		{
			return SDL_GetTicks() - start_ticks;
		}
		
	} 
	else
	{
		return end_time;
	}
}

void Timer::Resume()
{
	if (paused)
	{
		paused = false;
		start_ticks = SDL_GetTicks();
	}
}

float Timer::Pause()
{
	if (!paused)
	{
		paused = true;
		pause_time = SDL_GetTicks() - start_ticks;
	}	
	return pause_time;
}

bool Timer::IsPaused() const
{
	return paused;
}

float Timer::Stop()
{
	end_time = SDL_GetTicks() - start_ticks;
	running = false;
	paused = false;

	return end_time;
}