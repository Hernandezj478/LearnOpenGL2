#pragma once

struct v2
{
	float t1;
	float t2;
	v2()
	{
		t1 = 0.0f;
		t2 = 0.0f;
	}
	v2(float t1, float t2)
	{
		this->t1 = t1;
		this->t2 = t2;
	}

	bool operator==(const v2& other) const
	{
		return  t1 == other.t1 &&
				t2 == other.t2;
	}
};

struct v3
{
	float x;
	float y;
	float z;
	v3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	v3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	bool operator==(const v3& other) const
	{
		return	x == other.x &&
				y == other.y &&
				z == other.z;
	}
};
