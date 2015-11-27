#pragma once
#include <functional>

namespace Task {
	void InitializeThreadPool(int size);
	void StopThreadPool();

	void Enqueue(std::function<void()> task);
}