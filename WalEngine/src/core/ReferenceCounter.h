#pragma once

class ReferenceCounter
{
public:
	ReferenceCounter() :
		mRefCount(1) {}
	virtual ~ReferenceCounter() {}

	int get_reference() { return mRefCount; }
	void add_reference() { mRefCount++; }
	bool remove_reference() { mRefCount--; return mRefCount == 0; }

private:
	int mRefCount;
};
