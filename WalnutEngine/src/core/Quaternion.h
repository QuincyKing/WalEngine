#pragma once

#include <glm/glm.hpp>

class Quaternion
{
public:
	glm::vec4 que;

public:
	Quaternion(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 1.0f)
	{
		que.x = _x; que.y = _y; que.z = _z; que.w = _w;
	}

	Quaternion(const glm::vec4& r)
	{
		que.x = r.x; que.y = r.y; que.z = r.z; que.w = r.w;
	}

	Quaternion(const glm::vec3& axis, float angle)
	{
		float sinHalfAngle = sinf(angle / 2);
		float cosHalfAngle = cosf(angle / 2);

		que.x = axis.x * sinHalfAngle;
		que.y = axis.y * sinHalfAngle;
		que.z = axis.z * sinHalfAngle;
		que.w = cosHalfAngle;
	}

	Quaternion(const glm::mat4& m)
	{
		float trace = m[0][0] + m[1][1] + m[2][2];

		if (trace > 0)
		{
			float s = 0.5f / sqrtf(trace + 1.0f);
			que.w = 0.25f / s;
			que.x = (m[1][2] - m[2][1]) * s;
			que.y = (m[2][0] - m[0][2]) * s;
			que.z = (m[0][1] - m[1][0]) * s;
		}
		else if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
		{
			float s = 2.0f * sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]);
			que.w = (m[1][2] - m[2][1]) / s;
			que.x = 0.25f * s;
			que.y = (m[1][0] + m[0][1]) / s;
			que.z = (m[2][0] + m[0][2]) / s;
		}
		else if (m[1][1] > m[2][2])
		{
			float s = 2.0f * sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]);
			que.w = (m[2][0] - m[0][2]) / s;
			que.x = (m[1][0] + m[0][1]) / s;
			que.y = 0.25f * s;
			que.z = (m[2][1] + m[1][2]) / s;
		}
		else
		{
			float s = 2.0f * sqrtf(1.0f + m[2][2] - m[1][1] - m[0][0]);
			que.w = (m[0][1] - m[1][0]) / s;
			que.x = (m[2][0] + m[0][2]) / s;
			que.y = (m[1][2] + m[2][1]) / s;
			que.z = 0.25f * s;
		}

		float length = glm::length(que);

		que.w = que.w / length;
		que.x = que.x / length;
		que.y = que.y / length;
		que.z = que.z / length;
	}

	inline static glm::vec4 lerp(const glm::vec4 &l, const glm::vec4& r, float lerpFactor)
	{ 
		return (r - l) * lerpFactor + l;
	}

	static Quaternion nlerp(const Quaternion &l, const Quaternion& r, float lerpFactor, bool shortestPath)
	{
		Quaternion correctedDest;

		if (shortestPath && dot(l.que, r.que) < 0)
			correctedDest = r * -1;
		else
			correctedDest = r;

		return Quaternion(glm::normalize(lerp(l.que, correctedDest.que, lerpFactor)));
	}

	static Quaternion slerp(const Quaternion &l, const Quaternion& r, float lerpFactor, bool shortestPath)
	{
		static const float EPSILON = 1e3;

		float cos = glm::dot(l.que, r.que);
		Quaternion correctedDest;

		if (shortestPath && cos < 0)
		{
			cos *= -1;
			correctedDest = r * -1;
		}
		else
			correctedDest = r;

		if (fabs(cos) > (1 - EPSILON))
			return nlerp(l, correctedDest, lerpFactor, false);

		float sin = (float)sqrtf(1.0f - cos * cos);
		float angle = atan2(sin, cos);
		float invSin = 1.0f / sin;

		float srcFactor = sinf((1.0f - lerpFactor) * angle) * invSin;
		float destFactor = sinf((lerpFactor)* angle) * invSin;

		return Quaternion(l.que * srcFactor + correctedDest.que * destFactor);
	}

	static glm::mat4 rotation_from_vec(const glm::vec3& n, const glm::vec3& v, const glm::vec3& u)
	{
		glm::mat4 m;

		m[0][0] = u.x;   m[1][0] = u.y;   m[2][0] = u.z;   m[3][0] = 0.0;
		m[0][1] = v.x;   m[1][1] = v.y;   m[2][1] = v.z;   m[3][1] = 0.0;
		m[0][2] = n.x;   m[1][2] = n.y;   m[2][2] = n.z;   m[3][2] = 0.0;
		m[0][3] = 0.0;   m[1][3] = 0.0;   m[2][3] = 0.0;   m[3][3] = 1.0;

		return m;
	}

	static glm::mat4 rotation_from_direction(const glm::vec3& forward, const glm::vec3& up)
	{
		glm::vec3 n = glm::normalize(forward);
		glm::vec3 u = glm::cross(glm::normalize(up) ,n);
		glm::vec3 v = glm::cross(n, u);

		return rotation_from_vec(n, v, u);
	}

	inline glm::mat4 to_rotation_mat() const
	{
		glm::vec3 forward = glm::vec3(2.0f * (que.x * que.z - que.w * que.y), 2.0f * (que.y * que.z + que.w * que.x), 1.0f - 2.0f * (que.x * que.x + que.y * que.y));
		glm::vec3 up = glm::vec3(2.0f * (que.x*que.y + que.w*que.z), 1.0f - 2.0f * (que.x*que.x + que.z*que.z), 2.0f * (que.y*que.z - que.w*que.x));
		glm::vec3 right = glm::vec3(1.0f - 2.0f * (que.y*que.y + que.z*que.z), 2.0f * (que.x*que.y - que.w*que.z), 2.0f * (que.x*que.z + que.w*que.y));

		return rotation_from_vec(forward, up, right);
	}

	static glm::vec3 rotate(const glm::vec3 v, const Quaternion& rotation)
	{
		Quaternion conjugateQ = rotation.conjugate();
		Quaternion w = rotation * v * conjugateQ;

		glm::vec3 ret(w.que.x, w.que.y, w.que.z);

		return ret;
	}

	inline glm::vec3 get_forward() const
	{
		return rotate(glm::vec3(0, 0, 1), que);
	}

	inline glm::vec3 get_back() const
	{
		return rotate(glm::vec3(0, 0, -1), que);
	}

	inline glm::vec3 get_up() const
	{
		return rotate(glm::vec3(0, 1, 0), que);
	}

	inline glm::vec3 get_down() const
	{
		return rotate(glm::vec3(0, -1, 0), que);
	}

	inline glm::vec3 get_right() const
	{
		return rotate(glm::vec3(1, 0, 0), que);
	}

	inline glm::vec3 get_left() const
	{
		return rotate(glm::vec3(-1, 0, 0), que);
	}

	inline Quaternion conjugate() const { return Quaternion(-que.x, -que.y, -que.z, que.w); }

	Quaternion normalize()
	{
		return Quaternion(glm::normalize(que));
	}

	inline Quaternion operator*(const Quaternion& r) const
	{
		const float _w = (que.w * r.que.w) - (que.x * r.que.x) - (que.y * r.que.y) - (que.z * r.que.z);
		const float _x = (que.x * r.que.w) + (que.w * r.que.x) + (que.y * r.que.z) - (que.z * r.que.y);
		const float _y = (que.y * r.que.w) + (que.w * r.que.y) + (que.z * r.que.x) - (que.x * r.que.z);
		const float _z = (que.z * r.que.w) + (que.w * r.que.z) + (que.x * r.que.y) - (que.y * r.que.x);

		return Quaternion(_x, _y, _z, _w);
	}

	inline Quaternion operator*(const glm::vec3& v) const
	{
		const float _w = -(que.x * v.x) - (que.y * v.y) - (que.z * v.z);
		const float _x = (que.w * v.x) + (que.y * v.z) - (que.z * v.y);
		const float _y = (que.w * v.y) + (que.z * v.x) - (que.x * v.z);
		const float _z = (que.w * v.z) + (que.x * v.y) - (que.y * v.x);

		return Quaternion(_x, _y, _z, _w);
	}

	inline bool operator==(const Quaternion& r) const
	{
		for (unsigned int i = 0; i < 4; i++)
			if (que[i] != r.que[i])
				return false;
		return true;
	}

	inline bool operator!=(const Quaternion& r) const { return !operator==(r); }
};

