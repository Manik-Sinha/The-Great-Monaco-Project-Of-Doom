#pragma once
#include <mutex>

#define LOCK_SCOPE(lrObj, outRef) \
	std::lock_guard<std::mutex> guard##lrObj(lrObj.lock); \
	decltype(lrObj)::ResType & outRef = lrObj.data;

#define LOCK_SCOPE3(src, nametmp, outRef) \
	std::lock_guard<std::mutex> guard##nametmp(src.lock); \
	decltype(src)::ResType & outRef = src.data;

template <class ty>
class LockedResource
{
public:
	typedef ty ResType;

	// There's no effective way (that I could find (wleu 03/04/2021))
	// to automate this with a lock_guard while keeping these non-public.
	// But try not to use these data members directly, either swap or
	// use the LOCK_SCOPE macro.

	std::mutex lock;
	ty data;

public:
	void Swap(ty& dst)
	{
		lock.lock();
		std::swap(dst, this->data);
		lock.unlock();
	}
};