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
 
#include "socket.hpp"
using namespace network;

Socket::Socket():
    _status(Socket::Closed),
    _socket(-1)
{}

Socket::~Socket()
{
    if (this->_status != Socket::Closed) {
        this->close();
    }
}

int
Socket::open()
{
    return -1;
}

int
Socket::close()
{
    if (this->_status == Socket::Closed) {
        return -1;
    }
    
    // Close the socket
    int result = lwip_close(this->_socket);
    this->_socket = -1;
    
    // Update status and return
    this->_status = Socket::Closed;
    return result;
}

int
Socket::bind(int port)
{
    // Check socket status
    if (this->_status != Socket::Open) {
        return -1;
    }
    
    // Update local endpoint and create native
    struct sockaddr_in native_endpoint;
    this->_local_endpoint.setAddress(ip::Address(ip::Address::Any));
    this->_local_endpoint.setPort(port);
    this->_local_endpoint.toNative(&native_endpoint);
    
    // Bind socket to endpoint
    if (lwip_bind(this->_socket,
        (const struct sockaddr *)&native_endpoint,
        sizeof(native_endpoint)) < 0)
    {
        return -1;
    }

    return 0;
}

ip::Endpoint &
Socket::getRemoteEndpoint()
{
    return this->_remote_endpoint;
}

ip::Endpoint &
Socket::getLocalEndpoint()
{
    return this->_local_endpoint;
}

int
Socket::getHandle()
{
    return this->_socket;
}

enum Socket::Status
Socket::getStatus()
{
    return this->_status;
}
