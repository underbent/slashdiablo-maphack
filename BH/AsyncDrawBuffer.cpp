#include "AsyncDrawBuffer.h"
#include <Windows.h>
#include <vector>

class DrawItem {
public:
	std::function<void()> drawFunc;

	DrawItem(std::function<void()> _drawFunc) :
		drawFunc(_drawFunc) {}
};

class DrawBuffer {
private:
	CRITICAL_SECTION cSec;
public:
	std::vector<DrawItem> drawItems;

	DrawBuffer(){
		InitializeCriticalSection(&cSec);
	}

	~DrawBuffer(){
		DeleteCriticalSection(&cSec);
	}

	void draw(){
		lock();
		for (auto it = drawItems.begin(); it != drawItems.end(); it++){
			it->drawFunc();
		}
		unlock();
	}

	void clear(){
		//lock();
		drawItems.clear();
		//unlock();
	}

	void lock(){
		EnterCriticalSection(&cSec);
	}

	void unlock(){
		LeaveCriticalSection(&cSec);
	}
};

AsyncDrawBuffer::AsyncDrawBuffer()
{
	fg = new DrawBuffer();
	bg = new DrawBuffer();
}


AsyncDrawBuffer::~AsyncDrawBuffer()
{
	delete bg;
	delete fg;
}


// Calls all buffered draws
void AsyncDrawBuffer::drawAll()
{
	fg->draw();
}


// Pushes a draw function into the buffer
void AsyncDrawBuffer::push(std::function<void()> drawCall)
{
	bg->drawItems.push_back(DrawItem(drawCall));
}

void AsyncDrawBuffer::clear()
{
	bg->clear();
}

void AsyncDrawBuffer::swapBuffers()
{
	auto f = fg;
	f->lock();
	fg = bg;
	bg = f;
	f->unlock();
}

DrawDirective::DrawDirective(unsigned char _maxGhost) :
	frameCount(0),
	maxGhost(_maxGhost),
	updatePending(false),
	forcedUpdate(false)
{
}

DrawDirective::~DrawDirective()
{
}

void DrawDirective::forceUpdate(){
	forcedUpdate = true;
}

void DrawDirective::draw(fpDirector director)
{
	if (forcedUpdate || !updatePending && frameCount > maxGhost){
		updatePending = true;
		forcedUpdate = false;
		Task::Enqueue([=]()->void{
			buffer.clear();

			// The guts of the drawing
			director(buffer);

			buffer.swapBuffers();
			updatePending = false;
			frameCount = 0;
		});
	}

	buffer.drawAll();
	frameCount++;
}