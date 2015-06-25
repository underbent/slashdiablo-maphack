#include "Task.h"
#include <Windows.h>
#include <concurrent_queue.h>
#include <vector>

class TaskHolder {
public:
	std::function<void()> task;

	TaskHolder(std::function<void()> _task){
		task = _task;
	}
};

class WorkerThread {
public:
	bool canceled;
	HANDLE tHandle;

	WorkerThread() :
		canceled(false),
		tHandle(NULL){
	}

	~WorkerThread(){
		canceled = true;
		CloseHandle(tHandle);
	}
};

Concurrency::concurrent_queue<TaskHolder*> taskQueue;
std::vector<std::unique_ptr<WorkerThread>> threadPool;
HANDLE waitHandle = NULL;

namespace Task {
	DWORD WINAPI QueueThread(void* arg){
		WorkerThread *token = (WorkerThread*)arg;
		
		TaskHolder *t;
		while (!token->canceled){
			WaitForSingleObject(waitHandle, 2000);
			if (token->canceled) break;

			if (taskQueue.try_pop(t)){
				if (!taskQueue.empty()) {
					// Let the next task start while this one is processing
					SetEvent(waitHandle);
				}

				t->task();
				delete t;
			}
		}
		return 0;
	}

	void InitializeThreadPool(int size){
		waitHandle = CreateEvent(NULL, false, false, NULL);

		for (int i = 0; i < size; i++){
			WorkerThread *t = new WorkerThread();

			HANDLE tHandle = CreateThread(0, 0, QueueThread, (void*)t, 0, 0);
			t->tHandle = tHandle;
			threadPool.push_back(std::unique_ptr<WorkerThread>(t));

		}
	}

	void StopThreadPool(){
		CloseHandle(waitHandle);
		threadPool.clear();
	}

	void Enqueue(std::function<void()> task){
		taskQueue.push(new TaskHolder(task));
		SetEvent(waitHandle);
	}
}