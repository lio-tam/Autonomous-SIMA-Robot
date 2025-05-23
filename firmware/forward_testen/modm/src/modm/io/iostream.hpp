/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011-2017, 2019 Niklas Hauser
 * Copyright (c) 2012, 2015-2016, Sascha Schade
 * Copyright (c) 2015-2016, Kevin Läufer
 * Copyright (c) 2017, Marten Junga
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_IOSTREAM_HPP
#define MODM_IOSTREAM_HPP

#include <cstddef>
#include <modm/architecture/utils.hpp>

#include <stdarg.h>	// va_list
#include <inttypes.h>
#include <type_traits>
#include <climits>
#include <chrono>
#include <string_view>

#include "iodevice.hpp"
#include "iodevice_wrapper.hpp" // convenience

/// @cond
extern "C"
{
struct printf_output_gadget_t
{
	void (*function)(char, void*);
	void* extra_function_arg;
	char* buffer;
	unsigned int pos;
	unsigned int max_chars;
};
}
/// @endcond
namespace modm
{

/**
 * @ingroup modm_io
 * @author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 * @author	Niklas Hauser
 */
class IOStream
{
public:
	/**
	 * @param	device	device to write the stream to
	 *
	 * @code
	 *	MyIODevice device;
	 *	IOStream stream( device );
	 * @endcode
	 */
	inline IOStream(IODevice& odevice) :
		device(&odevice)
	{}

	// Acccessors -------------------------------------------------------------
	inline IOStream&
	write(char c)
	{ device->write(c); return *this; }

	static constexpr char eof = -1;

	/// Reads one character and returns it if available. Otherwise, returns IOStream::eof.
	inline IOStream&
	get(char& c)
	{
		if(!device->read(c)) {
			c = IOStream::eof;
		}
		return *this;
	}

	/// reads characters into NULL delimited c string
	/// in contrast to the standard implementation, this does not care about
	/// newline characters in the input
	IOStream&
	get(char* s, size_t n);

	template<size_t N>
	inline IOStream&
	get(char (&s)[N])
	{ return get(s, N); }

	// Modes ------------------------------------------------------------------
	inline IOStream&
	flush()
	{
		device->flush();
		mode = Mode::Ascii;
		return *this;
	}

	inline IOStream&
	endl()
	{
		mode = Mode::Ascii;
		device->write('\n');
		return *this;
	}

	/// set the output mode to binary style for integer types
	inline IOStream&
	bin()
	{ mode = Mode::Binary; return *this; }

	/// set the output mode to hexadecimal style for integer types
	inline IOStream&
	hex()
	{ mode = Mode::Hexadecimal; return *this; }

	/// set the output mode to ASCII style for integer types
	inline IOStream&
	ascii()
	{ mode = Mode::Ascii; return *this; }

	// Types ------------------------------------------------------------------
	IOStream&
	operator << (const bool& v);

	// char is equal to int8_t!
	inline IOStream&
	operator << (const char& v)
	{
		if (mode == Mode::Ascii)
			device->write(v);
		else if (mode == Mode::Binary)
			writeBin(static_cast<uint8_t>(v));
		else
			writeHex(static_cast<uint8_t>(v));
		return *this;
	}
	inline IOStream&
	operator << (const uint8_t& v)
	{
		if (mode == Mode::Ascii)
			writeInteger(static_cast<uint16_t>(v));
		else if (mode == Mode::Binary)
			writeBin(v);
		else
			writeHex(v);
		return *this;
	}

	inline IOStream& operator << (const int16_t& v)
	{ writeIntegerMode(v); return *this; }
	inline IOStream& operator << (const uint16_t& v)
	{ writeIntegerMode(v); return *this; }

	inline IOStream& operator << (const int32_t& v)
	{ writeIntegerMode(v); return *this; }
	inline IOStream& operator << (const uint32_t& v)
	{ writeIntegerMode(v); return *this; }

#ifdef __arm__
#ifdef __linux__
	// This overload is used on arm32v7. See issue with OSX below
	inline IOStream& operator << (const long unsigned int& v)
	{ writeIntegerMode(static_cast<uint32_t>(v)); return *this; }
#endif
#endif
	inline IOStream& operator << (const int64_t& v)
	{ writeIntegerMode(v); return *this; }
	inline IOStream& operator << (const uint64_t& v)
	{ writeIntegerMode(v); return *this; }

	// For ARM 'int32_t' is of type 'long'. Therefore there is no
	// function here for the default type 'int'. As 'int' has the same
	// width as 'int32_t' we just use a typedef here.
	inline IOStream& operator << (const int& v)
	{ writeIntegerMode(static_cast<int32_t>(v)); return *this; }
	inline IOStream& operator << (const unsigned int& v)
	{ writeIntegerMode(static_cast<uint32_t>(v)); return *this; }
	inline IOStream&
	operator << (const float& v)
	{ writeFloat(v); return *this; }

	inline IOStream&
	operator << (const double& v)
	{ writeDouble(v); return *this; }
	inline IOStream&
	operator << (const char* s)
	{ device->write(s); return *this; }

	inline IOStream&
	operator << (const std::string_view sv)
	{
		for (const auto c : sv) device->write(c);
		return *this;
	}

	/// write the hex value of a pointer
	inline IOStream&
	operator << (const void* p)
	{ writePointer(p); return *this; }

	/// Write the hex value of any function pointer
	template<class Ret, class... Args>
	IOStream&
	operator << (Ret(*pointer)(Args...))
	{ writePointer(reinterpret_cast<const void*>(&pointer)); return *this; }

	inline IOStream&
	operator << (IOStream& (*format)(IOStream&))
	{ return format(*this); }

	// printf -----------------------------------------------------------------
	IOStream&
	printf(const char* fmt, ...)  __attribute__((format(printf, 2, 3)));

	IOStream&
	vprintf(const char *fmt, va_list vlist) __attribute__((format(printf, 2, 0)));
protected:
	template< typename T >
	void
	writeIntegerMode(const T v)
	{
		constexpr size_t t_bits = sizeof(T)*8;
		if (mode == Mode::Ascii) {
			writeInteger(v);
		} else if (mode == Mode::Binary) {
			for (uint8_t ii=t_bits-8; ii < t_bits; ii -= 8)
				writeBin(static_cast<std::make_unsigned_t<T>>(v) >> ii);
		} else {
			for (uint8_t ii=t_bits-8; ii < t_bits; ii -= 8)
				writeHex(static_cast<std::make_unsigned_t<T>>(v) >> ii);
		}
	}

	void writeInteger(int16_t value);
	void writeInteger(uint16_t value);
	void writeInteger(int32_t value);
	void writeInteger(uint32_t value);
	void writeInteger(int64_t value);
	void writeInteger(uint64_t value);
	inline void writeFloat(float value)
	{ writeDouble(static_cast<double>(value)); }
	void writeDouble(const double& value);
	void writePointer(const void* value);
	void writeHex(uint8_t value);
	void writeBin(uint8_t value);

private:
	enum class
	Mode
	{
		Ascii,
		Hexadecimal,
		Binary
	};

	IOStream(const IOStream&) = delete;
	IOStream& operator =(const IOStream&) = delete;

private:
	IODevice* const	device;
	Mode mode = Mode::Ascii;
	static void out_char(char c, void* arg)
	{ if (c) reinterpret_cast<modm::IOStream*>(arg)->write(c); }
	printf_output_gadget_t output_gadget{out_char, this, NULL, 0, INT_MAX};
};

/// @ingroup modm_io
/// @{

/// Flushes the output stream.
/// This manipulator simply calls the stream's flush() member function.
inline IOStream&
flush(IOStream& ios)
{ return ios.flush(); }

/// Write a newline. **DOES NOT FLUSH THE STREAM!**
inline IOStream&
endl(IOStream& ios)
{ return ios.endl(); }
/// set the output mode to binary style
inline IOStream&
bin(IOStream& ios)
{ return ios.bin(); }

/// set the output mode to hexadecimal style
inline IOStream&
hex(IOStream& ios)
{ return ios.hex(); }

/// set the output mode to ASCII style
inline IOStream&
ascii(IOStream& ios)
{ return ios.ascii(); }

/// Set the foreground colour on ANSI terminals.
IOStream&
black(IOStream& ios);

IOStream&
red(IOStream& ios);

IOStream&
green(IOStream& ios);

IOStream&
yellow(IOStream& ios);

IOStream&
blue(IOStream& ios);

IOStream&
magenta(IOStream& ios);

IOStream&
cyan(IOStream& ios);

IOStream&
white(IOStream& ios);
/// @}

}	// namespace modm

#include "iostream_chrono.hpp"

#endif // MODM_IOSTREAM_HPP