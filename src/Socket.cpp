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
#include "Socket.h"
#include "Exceptions.h"
#include "Util.h"

// For getaddrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// Function: GetAddress
//
// Arguments:
//  saddr - sockaddr_t structure
//
// Description:
// Gets the address from the sockaddr_t structure (either IP standard)
// and returns it as a C++ string or an empty string on failure.
std::string GetAddress(sockaddr_t saddr)
{
	static char str[INET6_ADDRSTRLEN+1];
	memset(str, 0, sizeof(str));
	switch (saddr.sa.sa_family)
	{
		case AF_INET: return inet_ntop(AF_INET, &saddr.ipv4.sin_addr, str, INET6_ADDRSTRLEN);
		case AF_INET6: return inet_ntop(AF_INET6, &saddr.ipv6.sin6_addr, str, INET6_ADDRSTRLEN);
		default: return "";
	}

	return "";
}

// Function: GetPort
//
// Arguments:
//  s - sockaddr_t structure
//
// Description:
// Gets the address from the sockaddr_t structure (either IP standard)
// and returns it as a short integer.
short GetPort(sockaddr_t s)
{
	switch (s.sa.sa_family)
	{
		case AF_INET: return ntohs(s.ipv4.sin_port);
		case AF_INET6: return ntohs(s.ipv6.sin6_port);
		default: return -1;
	}
}

// Function: GetSockAddr
//
// Arguments:
//  type    - IPv4 or IPv6?
//  address - the IPv4/6 address
//  port    - obvious.
//
// Description:
// Converts the IP/Port combination into a sockaddr_t structure
// to be used with sockets.
sockaddr_t GetSockAddr(int type, const std::string &address, int port)
{
	sockaddr_t ret;
	memset(&ret, 0, sizeof(sockaddr_t));

	switch(type)
	{
	// IPv4 address.
		case AF_INET:
		{
		// Run through inet_pton and convert to binary.
			int i = inet_pton(type, address.c_str(), &ret.ipv4.sin_addr);
			if (i <= 0)
				return ret;
			else
			{
			// Set the type of socket (AF_INET) and put the port in
			// as big endianness (aka. network byte order)
				ret.ipv4.sin_family = type;
				ret.ipv4.sin_port = htons(port);
			}
		}
		case AF_INET6:
		{
			int i = inet_pton(type, address.c_str(), &ret.ipv6.sin6_addr);
			if (i <= 0)
				return ret;
			else
			{
				ret.ipv6.sin6_family = type;
				ret.ipv6.sin6_port = htons(port);
			}
		}
		default:
			break;
	};

	return ret;
}

// Function: ResolveDNS
//
// Arguments:
//  address - Address to resolve.
//  port    - port used (as string)
//
// Description:
// Gets a list of IP address structures for the DNS address.
std::vector<sockaddr_t*> ResolveDNS(const std::string &address, const std::string &port)
{
	std::vector<sockaddr_t*> addr;
	struct addrinfo hints, *result;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family   = AF_UNSPEC; // allow v4 or v6
	hints.ai_socktype = SOCK_STREAM; // TCP socket
	hints.ai_flags    = 0;
	hints.ai_protocol = 0;

	int ret = getaddrinfo(address.c_str(), port.c_str(), &hints, &result);
	if (ret != 0)
		throw SocketException("Cannot resolve %s:%s: %s", address, port, gai_strerror(ret));

	if (result == nullptr)
		throw SocketException("Cannot connect for DNS resolution.");

	for (struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next)
	{
		sockaddr_t *a = new sockaddr_t;
		memcpy(&a->sa, rp->ai_addr, sizeof(struct sockaddr));
		// Push into array with copy of the pointer since we're stack allocated.
		addr.push_back(a);
	}

	freeaddrinfo(result);

	return addr;
}

// Constructor: SecureConnectionSocket
//
// Arguments:
//  address - Address to resolve.
//  port    - port used (as string)
//
// Description:
// Opens an SSL socket to the specified address and port
SecureConnectionSocket::SecureConnectionSocket(const std::string &address, const std::string &port) : fd(0), address(address), port(port), ctx(nullptr), ssl(nullptr)
{
	// Initialize OpenSSL
    OpenSSL_add_all_algorithms();                      /* Load cryptos, et.al. */
    SSL_load_error_strings();                          /* Bring in and register error messages */
    const SSL_METHOD *method = SSLv23_client_method(); /* Create new client-method instance */
    this->ctx = SSL_CTX_new(method);                   /* Create new context */
    if (ctx == nullptr)
    {
		// Use a lambda expression because OpenSSL is dumb.
        std::string error = []() -> std::string
		{
			BIO *bio = BIO_new (BIO_s_mem ());
			ERR_print_errors (bio);
			char *buf = NULL;
			size_t len = BIO_get_mem_data (bio, &buf);
			char *ret = (char *)calloc (1, 1 + len);
			if (ret) memcpy (ret, buf, len);
			BIO_free (bio);
			std::string str = ret;
			free(ret);
			return str;
		}();

        throw SocketException("OpenSSL Error: %s", error);
    }

}

// Destructor: SecureConnectionSocket
//
// Arguments:
//  N/A
//
// Description:
// Closes SSL contexts, free's memory, and closes socket.
SecureConnectionSocket::~SecureConnectionSocket()
{
	SSL_free(this->ssl);
	::close(this->fd);
	SSL_CTX_free(this->ctx);
}

// Function: Connect
//
// Arguments:
//  <None>
//
// Description:
// Actually opens a connection to the address specified in the
// constructor
void SecureConnectionSocket::Connect()
{
	// Resolve our DNS address first.
	auto addresses = ResolveDNS(this->address, this->port);

	for (auto cur : addresses)
	{
		this->fd = ::socket(cur->sa.sa_family, SOCK_STREAM, 0);
		if (::connect(this->fd, &cur->sa, sizeof(struct sockaddr)) != 0)
		{
			// Failed to connect.
			::close(this->fd);
			this->fd = -1;
			continue;
		}
	}

	// Make sure we actually connected.
	if (this->fd == -1)
		throw SocketException("Failed to connect to a host");

	// Free our memory.
	for (auto addr : addresses)
		delete addr;

	// Now do SSL stuff.
	this->ssl = SSL_new(ctx);
	// Associate the fd with a SSL context.
	SSL_set_fd(this->ssl, this->fd);

	if (SSL_connect(ssl) < 0)
	{
		// Use a lambda expression because OpenSSL is dumb.
		std::string error = []() -> std::string
		{
			BIO *bio = BIO_new (BIO_s_mem ());
			ERR_print_errors (bio);
			char *buf = NULL;
			size_t len = BIO_get_mem_data (bio, &buf);
			char *ret = (char *)calloc (1, 1 + len);
			if (ret) memcpy (ret, buf, len);
			BIO_free (bio);
			std::string str = ret;
			free(ret);
			return str;
		}();

		throw SocketException("OpenSSL Error: %s", error);
	}

	// Everything is all good! We're good to go :3
}

// Function: Write
//
// Arguments:
//  data - Binary data to write
//  len  - size of the binary data
//
// Description:
// Writes data to the SSL socket, returns bytes written.
size_t SecureConnectionSocket::Write(const void *data, size_t len)
{
	return SSL_write(this->ssl, data, len);
}

// Function: Read
//
// Arguments:
//  data - binary data
//  len  - pointer to length of binary data
//
// Description:
// Reads data from the secure socket.
void SecureConnectionSocket::Read(void *data, size_t *len)
{
	assert(len);
	size_t buflen = *len;
	*len = SSL_read(this->ssl, data, buflen);
}
