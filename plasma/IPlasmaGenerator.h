#pragma once

#include <memory>

class IPlasmaGenerator
{
public:
	const static int BytesPerPixel = 4;

	virtual void SetSize(int width, int height)
	{
		m_data.reset(new unsigned char [width * height* BytesPerPixel]);
		m_width = width;
		m_height = height;
	}

	virtual unsigned char *Generate(unsigned time) = 0;

protected:
	int m_width;
	int m_height;
	std::unique_ptr<unsigned char[]> m_data;
};
