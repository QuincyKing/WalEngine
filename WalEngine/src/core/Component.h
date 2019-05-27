#pragma once

enum class ComType
{
	DirLit,
	PointLit,
	SpotLit
};

class Component
{
public:
	Component() = default;
	virtual ~Component() {}
	Component(const Component& other) {}
	void operator=(const Component& other) {}

	virtual void render() {};
};