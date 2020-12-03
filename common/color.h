/*====================================================================

 Purpose: Reinvented wheel: Color class, 4 bytes
 Copyright (c) 2011 Xawari
 Header is C and C++ compliant

//==================================================================*/
#ifndef COLOR_H
#define COLOR_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* !__MINGW32__ */
#endif
/*
struct color24bit_s
{
	byte r;
	byte g;
	byte b;
	byte a;
};

union color24bit
{
	color24bit_s bytes;
	byte array[4];
	unsigned int integer;// ABGR
};
*/
#ifdef __cplusplus

// RGBA 4 Bytes
class Color
{
public:
	union
	{
		unsigned int integer;// ABGR
		byte array[4];
		struct
		{
			byte r;
			byte g;
			byte b;
			byte a;
		};
	};

	Color()
	{
		SetColor(0x00000000);
	}
	Color(byte r, byte g, byte b)// bytes are [0...255]. Assuming alpha = 255 for use with RGBA functions
	{
		SetColor(r, g, b, 255);
	}
	Color(int r, int g, int b)// integers are [0...255]. Assuming alpha = 255 for use with RGBA functions
	{
		SetColor(r, g, b, 255);
	}
	Color(byte r, byte g, byte b, byte a)// bytes are [0...255]
	{
		SetColor(r, g, b, a);
	}
	Color(int r, int g, int b, int a)// integers are [0...255]
	{
		SetColor(r, g, b, a);
	}
	Color(byte array[])// bytes are [0...255]
	{
		SetColor(array[0], array[1], array[2], array[3]);
	}

	// Set
	void SetColor(const unsigned int &color4b)
	{
		integer = color4b;
	}
	void SetColor(const byte &br, const byte &bg, const byte &bb, const byte &ba)
	{
		r = br;
		g = bg;
		b = bb;
		a = ba;
	}
	void SetColor(const byte &br, const byte &bg, const byte &bb)// does NOT change alpha value
	{
		r = br;
		g = bg;
		b = bb;
	}
	void SetColor4f(const float &fr, const float &fg, const float &fb, const float &fa)// floats are [0...1]
	{
		r = (byte)(fr*255.0f);// should really use round()
		g = (byte)(fg*255.0f);
		b = (byte)(fb*255.0f);
		a = (byte)(fa*255.0f);
	}

	// Get
	void GetColor(byte &br, byte &bg, byte &bb, byte &ba) const
	{
		br = r;
		bg = g;
		bb = b;
		ba = a;
	}
	void GetColor(int &ir, int &ig, int &ib, int &ia) const
	{
		ir = r;
		ig = g;
		ib = b;
		ia = a;
	}
	void GetColor(int &color4b) const
	{
		color4b = integer;
	}
	void GetColor4f(float &fr, float &fg, float &fb, float &fa)// floats are [0...1]
	{
		fr = ((float)r/255.0f);
		fg = ((float)g/255.0f);
		fb = ((float)b/255.0f);
		fa = ((float)a/255.0f);
	}

	// Type conversion
	operator unsigned int()
	{
		return integer;
	}

	byte &operator[](int index)
	{
		return array[index];
//		return *((unsigned char *)(c.integer)+(sizeof(byte))*index);
	}

	// Equality operators
	bool operator == (Color &rhs) const
	{
		return (integer == rhs.integer);
	}
	bool operator != (Color &rhs) const
	{
		return (integer != rhs.integer);
	}

	// Assignment operators
	inline Color& operator-=(const Color &a);
	inline Color& operator+=(const Color &a);
	inline Color& operator*=(const float &f);
	inline Color& operator/=(const float &f);
	inline Color& operator*=(const Color &a);// a[i]*b[i]
	inline Color& operator-=(const byte &a);
	inline Color& operator+=(const byte &a);

	// Unary operators
	Color operator-() const { return Color(255-r, 255-g, 255-b, 255-a); }// does this make sence?
	Color operator+() const { return *this; }

	// Binary operators
	inline Color operator-(const Color&) const;
	inline Color operator+(const Color&) const;
	inline Color operator/(const Color&) const;
    inline Color operator*(const Color&) const;
	inline Color operator^(const Color&) const;// CROSS PRODUCT

	// Misc
	void SetBlack(void) { SetColor(0x000000FF); }
	void SetWhite(void) { SetColor(0xFFFFFFFF); }
};

// Assignment operators
inline Color& Color::operator-=(const Color &c)
{
	r -= c.r;
	g -= c.g;
	b -= c.b;
	a -= c.a;
	return *this;
}

inline Color& Color::operator+=(const Color &c)
{
	r += c.r;
	g += c.g;
	b += c.b;
	a += c.a;
	return *this;
}

inline Color& Color::operator*=(const float &f)
{
	r *= (int)f;
	g *= (int)f;
	b *= (int)f;
	a *= (int)f;
	return *this;
}

inline Color& Color::operator/=(const float &f)
{
	r /= (int)f;
	g /= (int)f;
	b /= (int)f;
	a /= (int)f;
	return *this; 
}

inline Color& Color::operator*=(const Color &c)
{
	r *= c.r;
	g *= c.g;
	b *= c.b;
	a *= c.a;
	return *this; 
}

inline Color& Color::operator-=(const byte &v)
{
	r -= v;
	g -= v;
	b -= v;
	a -= v;
	return *this;
}

inline Color& Color::operator+=(const byte &v)
{
	r += v;
	g += v;
	b += v;
	a += v;
	return *this;
}


// Binary operators
inline Color Color::operator-(const Color &a) const
{
	return (Color(	r - a.r,
					g - a.g,
					b - a.b));
}

inline Color Color::operator+(const Color &a) const
{
	return (Color(	r + a.r,
					g + a.g,
					b + a.b));
}

inline Color Color::operator/(const Color &a) const
{
	return (Color(	r / a.r,
					g / a.g,
					b / a.b));
}

inline Color Color::operator*(const Color &a) const
{
	return (Color(	r * a.r,
					g * a.g,
					b * a.b));
}

// Stand-alone operators
inline Color operator*(float f, const Color &c)
{
	return Color((byte)(c.r*f), (byte)(c.g*f), (byte)(c.b*f));//, (byte)(c.a*f));
}

inline Color operator*(const Color &c, float f)
{
	return Color((byte)(c.r*f), (byte)(c.g*f), (byte)(c.b*f));//, (byte)(c.a*f));
}

inline Color operator/(const Color &c, float f)
{
	return Color((byte)(c.r/f), (byte)(c.g/f), (byte)(c.b/f));//, (byte)(c.a/f));
}

inline Color operator+(const Color &c, byte f)
{
	return Color(c.r+f, c.g+f, c.b+f);//, c.a+f);
}

inline Color operator+(byte f, const Color &c)
{
	return Color(c.r+f, c.g+f, c.b+f);//, c.a+f);
}

inline Color operator-(const Color &c, byte f)
{
	return Color(c.r-f, c.g-f, c.b-f);//, c.a-f);
}

inline Color operator-(byte f, const Color &c)
{
	return Color(f-c.r, f-c.g, f-c.b);//, f-c.a);
}

// Some constants
#define ColorRed		Color(255,0,0,255);
#define ColorGreen		Color(0,255,0,255);
#define ColorBlue		Color(0,0,255,255);

void ScaleColorsF(int &r, int &g, int &b, const float &a);
void ScaleColors(int &r, int &g, int &b, const int &a);
void ScaleColorsF(byte &r, byte &g, byte &b, const float &a);
void ScaleColors(byte &r, byte &g, byte &b, const byte &a);

#else /* __cplusplus */

typedef struct Color_s
{
	color24bit c;
} Color;

#endif /* __cplusplus */


#endif /* COLOR_H */
