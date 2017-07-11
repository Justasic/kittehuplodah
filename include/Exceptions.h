/*
 * Copyright (c) 2017 Justin Crawford
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once
#include "tinyformat.h"

class BasicException : public std::exception
{
protected:
	const std::string err;
public:
	BasicException(const std::string &message) : err(message) {}

	// Formatted constructor for ease of use,
	template<typename... Args> BasicException(const std::string &message, const Args&... args) : err(tfm::format(message, args...)) { }

	virtual ~BasicException() throw() { };

	virtual const char* what() const noexcept
	{
		return this->err.c_str();
	}
};

class ConfigException : public BasicException
{
public:
	ConfigException(const std::string &err) : BasicException(err) { }
	template<typename... Args> ConfigException(const std::string &message, const Args&... args) : BasicException(message, args...) { }
};

class SocketException : public BasicException
{
public:
	SocketException(const std::string &err) : BasicException(err) { }
	template<typename... Args> SocketException(const std::string &message, const Args&... args) : BasicException(message, args...) { }

};
