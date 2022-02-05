#ifndef WORKER_POOL_H
#define WORKER_POOL_H

#include <queue>
#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>

template <class T>
class ThreadSafeQueue {
	std::queue<T> queue;
	std::mutex mutex;
public:
	void push(const T& value) {
		std::unique_lock<std::mutex> lock(this->mutex);
		this->queue.push(value);
	}

	T& front() {
		std::unique_lock<std::mutex> lock(this->mutex);
		return this->queue.front();
	}

	void pop() {
		std::unique_lock<std::mutex> lock(this->mutex);
		this->queue.pop();
	}

	bool empty() {
		std::unique_lock<std::mutex> lock(this->mutex);
		return this->queue.empty();
	}

	int size() {
		std::unique_lock<std::mutex> lock(this->mutex);
		return this->queue.size();
	}
};

class WorkerPool {
	std::queue <std::function<void()>> tasks;
	std::vector<std::thread> workers;
	std::mutex mutex;
	std::condition_variable condition;

	bool stop;

	void workerThread() {
		while (!this->stop) {
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(this->mutex);

				this->condition.wait(lock, [this] {
					return !this->tasks.empty();
				});

				if (!this->tasks.empty()) {
					task = this->tasks.front();
					this->tasks.pop();
				}
			}

			task();
		}
	};
public:
	WorkerPool(std::size_t workerCount) {
		this->stop = false;
		for (int i = 0; i < workerCount; i++)
			workers.push_back(std::thread(&WorkerPool::workerThread, this));
	}

	WorkerPool(const WorkerPool& other) = delete;
	WorkerPool(WorkerPool&& other) = delete;
	WorkerPool& operator=(const WorkerPool& other) = delete;
	WorkerPool& operator=(WorkerPool&& other) = delete;

	template<class F, class... Args>
	auto addTask(F&& f, Args&&... args)->std::future<decltype(f(args...))> {
		auto task = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		auto future = task->get_future();
		{
			std::unique_lock<std::mutex> lock(this->mutex);
			std::function<void()> wrapper = [task]() {
				(*task)();
			};
			this->tasks.push(wrapper);
		}

		this->condition.notify_one();

		return future;
	}

	~WorkerPool() {
		{
			std::unique_lock<std::mutex> lock(this->mutex);
			this->stop = true;
		}

		condition.notify_all();
		for (std::thread& worker : workers)
			worker.join();
	}
};

class TaskScheduler {
	WorkerPool workerPool;
	std::unordered_map<unsigned int, bool> inProgress;
	ThreadSafeQueue<std::function<void()>> callbacks;

	template <typename T>
	using Task = std::function<const T()>;
	template <typename T>
	using OnComplete = std::function<void(const T&)>;
	using OnError = std::function<void(const std::exception&)>;

public:
	TaskScheduler() : workerPool(std::thread::hardware_concurrency()) {}

	TaskScheduler(const TaskScheduler& other) = delete;
	TaskScheduler(TaskScheduler&& other) = delete;
	TaskScheduler& operator=(const TaskScheduler& other) = delete;
	TaskScheduler& operator=(TaskScheduler&& other) = delete;

	template<typename T>
	void dispatch(unsigned int id, Task<T> task, OnComplete<T> onComplete = {}, OnError onError = {}) {
		if (this->isInProgress(id))
			throw std::invalid_argument("Task '" + std::to_string(id) + "' is already in progress");

		this->inProgress[id] = true;

		this->workerPool.addTask([this, id, task, onComplete, onError]() {
			try {
				T data = task();
				if (onComplete) {
					std::function<void()> wrapper = [this, id, onComplete, data]() {
						onComplete(data);
						this->inProgress.erase(id);
					};

					this->callbacks.push(wrapper);
				} else {
					this->inProgress.erase(id);
				}
			} catch (const std::exception& e) {
				if (onError) {
					std::function<void()> wrapper = [this, id, onError, e]() {
						onError(e);
						this->inProgress.erase(id);
					};

					this->callbacks.push(wrapper);
				} else {
					this->inProgress.erase(id);
				}
			}
		});
	}

	void poll() {
		while (!this->callbacks.empty()) {
			std::function<void()> callback = callbacks.front();
			callbacks.pop();

			callback();
		}
	}

	bool isInProgress(unsigned int id) const {
		if (this->inProgress.find(id) != this->inProgress.end())
			return true;

		return false;
	}
};

#endif