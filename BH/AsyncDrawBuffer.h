#pragma once
#include "Task.h"

class DrawBuffer;
class DrawDirective;
class AsyncDrawBuffer;

typedef std::function<void(AsyncDrawBuffer&)> fpDirector;

class AsyncDrawBuffer
{
private:
	DrawBuffer* fg;
	DrawBuffer* bg;
public:
	AsyncDrawBuffer();
	~AsyncDrawBuffer();

	// Calls all buffered draw calls in the fore buffer
	void drawAll();

	// Pushes a draw function into the back buffer
	void push(std::function<void()> drawCall);

	// Clears the backbuffer
	void clear();

	// Switches the fore and back buffers to change the displayed frame
	void swapBuffers();
};

class DrawDirective {
private:
	int frameCount;
	unsigned char maxGhost;
	bool updatePending;
	AsyncDrawBuffer buffer;
	bool forcedUpdate;
public:
	DrawDirective(unsigned char _maxGhost);
	~DrawDirective();
	void draw(fpDirector director);
	void forceUpdate();
};

