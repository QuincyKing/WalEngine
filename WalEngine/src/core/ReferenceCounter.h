#pragma once

class ReferenceCounter
{
public:
	ReferenceCounter() :
		mRefCount(1) {}
	virtual ~ReferenceCounter() {}

	inline int get_reference() { return mRefCount; }
	inline void add_reference() { mRefCount++; }
	inline bool remove_reference() { mRefCount--; return mRefCount == 0; }

private:
	int mRefCount;
};
