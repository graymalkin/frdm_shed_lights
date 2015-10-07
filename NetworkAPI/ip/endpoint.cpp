/**
 * Copyright (c) 2012, Roy van Dam <roy@vandam-innovations.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include "endpoint.hpp"
using namespace network::ip;

Endpoint::Endpoint():
    _port(0)
{}

Endpoint::Endpoint(const Endpoint &other):
    _address(other._address),
    _port(other._port)
{}

Endpoint::Endpoint(const Address &address):
    _address(address),
    _port(0)
{}

Endpoint::Endpoint(const Address &address, int port):
    _address(address),
    _port(port)
{}

int
Endpoint::setPort(int port)
{
    if (port > 65536) {
        return -1;
    }
    
    this->_port = port;
    return 0;
}

int
Endpoint::getPort()
{
    return this->_port;
}

void
Endpoint::setAddress(const Address &address)
{
    this->_address = address;
}

Address &
Endpoint::getAddress()
{
    return this->_address;
}

int
Endpoint::toNative(struct sockaddr_in *endpoint)
{
    if (endpoint == NULL) {
        return 1;
    }
    
    // Clear structure
    std::memset(endpoint, 0, sizeof(struct sockaddr_in));
    
    // Export endpoint
    endpoint->sin_family = AF_INET;
    endpoint->sin_port = htons(this->_port);
    endpoint->sin_addr.s_addr = this->_address.toNative();
    return 0;
}

int
Endpoint::fromNative(struct sockaddr_in *endpoint)
{
    if (endpoint == NULL) {
        return 1;
    }
    
    // Import endpoint
    this->_port = ntohs(endpoint->sin_port);
    this->_address.fromNative(endpoint->sin_addr.s_addr);
    return 0;
}

Endpoint &
Endpoint::operator=(const Endpoint &other) {
    this->_address = other._address;
    this->_port = other._port;
    
    return (*this);
}