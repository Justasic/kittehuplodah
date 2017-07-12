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

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "CommandLine.h"
#include "Config.h"
#include "Util.h"
#include "Exceptions.h"
#include "Socket.h"

// Global: config
//
// Arguments:
//  N/A
//
// Description:
// Global pointer of the Config class (see Config.h)
// that allows all classes and applications to access
// the config values parsed by this class.
Config *config;

// Function: main
//
// Arguments:
//  argc - number of items in argv
//  argv - a char array of all command line options.
//
// Description:
// Function is the standard start of the application
// and contains all abstracted procedures to process
// command line arguments, read config, open a socket,
// send a file, and clean up before exiting.
int main(int argc, char **argv)
{
	// Start with parsing the command line.
	auto args = ProcessArgs(argc, argv);

	// Parse the config and set it's global.
	try
	{
		Config conf(args["config"]);
		config = &conf;
	}
	catch (const ConfigException &e)
	{
		tfm::printf("There was a problem reading the config file %s:\n%s\n", args["config"], e.what());
		return EXIT_FAILURE;
	}

	tfm::printf("Using uploader %s to connect to %s\n", config->uploader, config->uploadurl);

	auto url = DecodeURL(config->uploadurl);
	if (url["protocol"] == "https")
	{
		// Connect to https server.
		try
		{
			SecureConnectionSocket sock(url["hostname"], "443");
			// Okay! we're ready to start sending data :D







		} catch (const SocketException &e)
		{
			tfm::printf("There was a problem trying to connect to %s: \n%s\n", config->uploadurl, e.what());
			return EXIT_FAILURE;
		}
	}
	else
		tfm::printf("Sorry, %s is an unsupported protocol right now.\n", url["protocol"]);

	// Exit the application successfully.
	return EXIT_SUCCESS;
}
