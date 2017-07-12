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
#include <string>
#include <map>

// Function: DecodeURL
//
// Arguments:
//  url - A standard protocol url.
//
// Description:
// takes in a URL (eg. https://api.teknik.io/v1/Upload) and
// converts it into an associative map that is easier to use
// in code elsewhere.
std::map<std::string, std::string> DecodeURL(const std::string &url)
{
	std::map<std::string, std::string> parts;

	// First, find the :// part of the URL (eg http://)
	size_t protppos = url.find("://");
	if (protppos == std::string::npos)
		return parts;
	// Next find the next slash after ://.
	size_t hostpos  = url.substr(protppos+3).find("/");
	if (protppos == std::string::npos)
		return parts;


	parts["protocol"] = url.substr(0, protppos);
	parts["hostname"] = url.substr(protppos+3, hostpos);
	parts["path"]     = url.substr(hostpos+1);

	return parts;
}
