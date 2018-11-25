#pragma once

#include <stack>
#include <mutex>

template <class T>
class SharedPool
{
private:
	struct PoolDeleter 
    {
		explicit PoolDeleter(std::weak_ptr<SharedPool<T>* > pool) : _pool(pool) {}

		void operator()(T* ptr) 
        {
			// Put it back into the pool if the pool still exists
			if (auto pool_ptr = _pool.lock())
            {
				try 
                {
					(*pool_ptr.get())->add(std::unique_ptr<T>{ptr});
					return;
				}
				catch (...) {}
			}
			// Else delete the object
			std::default_delete<T>{}(ptr);
		}
	private:
		std::weak_ptr<SharedPool<T>* > _pool;
	};

public:
	using ptr_type = std::unique_ptr<T, PoolDeleter >;

	SharedPool() : _thisPtr(new SharedPool<T>*(this)), _mutex() {}
	virtual ~SharedPool() {}

	void add(std::unique_ptr<T> t) 
    {
		std::lock_guard<std::mutex> lock(_mutex);
        _pool.push(std::move(t));
	}

	ptr_type acquire() 
    {
		std::lock_guard<std::mutex> lock(_mutex);
		assert(!_pool.empty());
		ptr_type tmp(_pool.top().release(), PoolDeleter{ std::weak_ptr<SharedPool<T>*>{_thisPtr} });
        _pool.pop();
		return std::move(tmp);
	}

	bool empty() 
    {
		std::lock_guard<std::mutex> lock(_mutex);
		return _pool.empty();
	}

	size_t size() 
    {
		std::lock_guard<std::mutex> lock(_mutex);
		return _pool.size();
	}

private:
	std::shared_ptr<SharedPool<T>* > _thisPtr;
	std::stack<std::unique_ptr<T> > _pool;
	std::mutex _mutex;
};