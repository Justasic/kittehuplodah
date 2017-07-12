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
#pragma once
// For inet_ntop and inet_pton
#include <arpa/inet.h>
// For SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <cassert>
#include <cstring>
#include <vector>
#include <string>

typedef union {
	struct sockaddr_in ipv4;
	struct sockaddr_in6 ipv6;
	struct sockaddr sa;
} sockaddr_t;

class SecureConnectionSocket
{
protected:
	// The file descriptor of our socket.
	int fd;
	// The address we're connected to.
	std::string address;
	// Port we're using.
	std::string port;
	// OpenSSL contexts
	SSL_CTX *ctx;
	SSL *ssl;
public:
	// Constructors/destructors
	SecureConnectionSocket() = delete; // We delete this constructor to prevent opject copies.
	SecureConnectionSocket(const std::string &address, const std::string &port);
	~SecureConnectionSocket();

	// Control functions.
	void Connect();

	// Read and write functions.
	size_t Write(const void *data, size_t len);
	void Read(void *data, size_t *len);

	// Getters/setters.
	inline std::string GetAddress() const { return this->address; }
	inline std::string GetPort() const { return this->port; }
	inline int GetFD() const { return this->fd; }
};
