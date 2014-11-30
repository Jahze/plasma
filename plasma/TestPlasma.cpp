#define _USE_MATH_DEFINES
#include <cassert>
#include <cmath>
#include "TestPlasma.h"

#define _USE_MATH_DEFINES

namespace
{
	unsigned char Scale(double x)
	{
		x += 1.0;
		x *= 127.5;
		return (unsigned char)x;
	}

	unsigned char ScaleByte(unsigned char b, double s)
	{
		return (unsigned char)((double)b * s);
	}

	double ColorPhaseFactor(double (*func)(double), double phase, double min, double max)
	{
		if (phase > max || phase < min)
			return 0;

		phase -= min;
		phase /= (max - min);
		return (*func)((phase * M_PI_2) + M_PI_4);
	}
}

unsigned char* TestPlasma::Generate(unsigned time)
{
	auto phase = (unsigned char)(time >> 6);
	double dphase = (double)phase;
	auto red = ColorPhaseFactor(tan, dphase, 0.0, 128.0);
	auto green = ColorPhaseFactor(tan, dphase, 64.0, 192.0);
	auto blue = ColorPhaseFactor(tan, dphase, 128.0, 255.0);
	

	for (int i = 0; i < m_height; ++i)
	{
		for (int j = 0; j < m_width; j++)
		{
			int idx = (i * m_width) + j;

			double jval = (double)(j-phase) / 16;
			double ival = (double)(i) / 16;

			unsigned char value = (Scale(sin(ival)) + Scale(sin(jval))) / 2;

			m_data[idx*BytesPerPixel+0] = ScaleByte(value, red);
			m_data[idx*BytesPerPixel+1] = ScaleByte(value, green);
			m_data[idx*BytesPerPixel+2] = ScaleByte(value, blue);
			m_data[idx*BytesPerPixel+3] = 0x00;
		}
	}

	return m_data.get();
}
