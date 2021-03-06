/*
 * Copyright (c) 2017 Justin Crawford and NamedKitten
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

#include <unistd.h>
#include "Config.h"
#include "Exceptions.h"
#include "inih/INIReader.h"

// Constructor: Config class
//
// Arguments:
//  cf - Config File location (relative or absolute path)
//
// Description:
// Opens and reads the config file then places parsed values into
// the class's members.
Config::Config(const std::string &cf) : ConfigFile(cf)
{
	INIReader reader(cf);

	if (reader.ParseError() < 0)
		throw ConfigException("There was an error reading config '%s'.", cf);

	// Parse everything!
	this->uploader = reader.Get("default", "uploader", "\007UNKNOWN\007");

	if (this->uploader == "\007UNKNOWN\007")
		throw ConfigException("Cannot have unknown value for 'uploader' config option\n");

	this->uploadurl = reader.Get(this->uploader, "url", "\007UNKNOWN\007");

	if (this->uploadurl == "\007UNKNOWN\007")
		throw ConfigException("Cannot have unknown value for 'url' config option\n");
}

Config::~Config()
{

}
