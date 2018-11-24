#pragma once

#include <stack>
#include <mutex>

template <class T>
class SharedPool
{
private:
	struct PoolDeleter {
		explicit PoolDeleter(std::weak_ptr<SharedPool<T>* > pool)
			: pool_(pool) {}

		void operator()(T* ptr) {
			// Put it back into the pool if the pool still exists
			if (auto pool_ptr = pool_.lock()) {
				try {
					(*pool_ptr.get())->add(std::unique_ptr<T>{ptr});
					return;
				}
				catch (...) {}
			}
			// Else delete the object
			std::default_delete<T>{}(ptr);
		}
	private:
		std::weak_ptr<SharedPool<T>* > pool_;
	};

public:
	using ptr_type = std::unique_ptr<T, PoolDeleter >;

	SharedPool() : 
		this_ptr_(new SharedPool<T>*(this)), 
		mutex_() {}
	virtual ~SharedPool() {}

	void add(std::unique_ptr<T> t) {
		std::lock_guard<std::mutex> lock(mutex_);
		pool_.push(std::move(t));
	}

	ptr_type acquire() {
		std::lock_guard<std::mutex> lock(mutex_);
		assert(!pool_.empty());
		ptr_type tmp(pool_.top().release(),
			PoolDeleter{ std::weak_ptr<SharedPool<T>*>{this_ptr_} });
		pool_.pop();
		return std::move(tmp);
	}

	bool empty() {
		std::lock_guard<std::mutex> lock(mutex_);
		return pool_.empty();
	}

	size_t size() {
		std::lock_guard<std::mutex> lock(mutex_);
		return pool_.size();
	}

private:
	std::shared_ptr<SharedPool<T>* > this_ptr_;
	std::stack<std::unique_ptr<T> > pool_;
	std::mutex mutex_;
};