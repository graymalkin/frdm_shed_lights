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
using namespace network::udp;

int
Socket::open()
{
    // Check socket status
    if (this->_status != Socket::Closed) {
        return -1;
    }
    
    // Open socket
    this->_socket = lwip_socket(AF_INET, SOCK_DGRAM, 0);
    if (this->_socket < 0) {
        return -1;
    }
    
    // Update status and return
    this->_status = Socket::Open;
    return 0;
}

int
Socket::send(Buffer &buffer, ip::Address &address, int port)
{
    ip::Endpoint endpoint(address, port);
    return this->send(buffer.data(), buffer.length(), endpoint);
}

int
Socket::send(Buffer &buffer, ip::Endpoint &endpoint)
{
    return this->send(buffer.data(), buffer.length(), endpoint);
}

int
Socket::send(void *data, size_t size, ip::Address &address, int port)
{
    ip::Endpoint endpoint(address, port);
    return this->send(data, size, endpoint);
}

int
Socket::send(void *data, size_t size, ip::Endpoint &endpoint)
{
    // Check data buffer and size
    if (data == NULL || size == 0) {
        return -1;
    }

    // Check socket status
    if (this->_status != Socket::Open) {
        return -1;
    }
    
    // Create native endpoint
    struct sockaddr_in native_endpoint;
    endpoint.toNative(&native_endpoint);
    
    // Update status
    this->_status = Socket::Sending;
    
    // Try to write the specified amount of bytes.
    int bytes_written = lwip_sendto(this->_socket, data, size, 0,
        (const struct sockaddr *)&native_endpoint, sizeof(native_endpoint));
    
    // Update status
    this->_status = Socket::Open;
    
    // Return the result.
    return bytes_written;
}

int
Socket::receive(Buffer &buffer)
{
    int result = this->receive(buffer.data(), buffer.size());
    if (result >= 0) {
        buffer.length(result);
    }
    
    return result;
}

int
Socket::receive(void *data, size_t max_size)
{
    return this->receive(data, max_size, this->_remote_endpoint);
}

int
Socket::receive(void *data, size_t max_size, ip::Endpoint &endpoint)
{
    // Check data buffer and size
    if (data == NULL || max_size == 0) {
        return -1;
    }

    // Check socket status
    if (this->_status != Socket::Open) {
        return -1;
    }

    // Create native endpoint
    struct sockaddr_in native_endpoint;
    int native_endpoint_size = sizeof(native_endpoint);
    std::memset(&native_endpoint, 0, sizeof(native_endpoint));
    
    // Update status
    this->_status = Socket::Receiving;
    
    // Try to read a packet from the socket.
    int bytes_read = lwip_recvfrom(this->_socket, data, max_size, 0,
        (struct sockaddr*)&native_endpoint, (u32_t *)&native_endpoint_size);
    
    // Update status and return
    this->_status = Socket::Open;
    
    // Did we succeed? 
    if (bytes_read < 0) {
        return -1;
    }
    
    // Check if we received the endpoint information correctly
    if (native_endpoint_size != sizeof(native_endpoint)) {
        printf("Warning: invalid endpoint size received\n\r");
        return bytes_read;
    }
    
    // Copy remote endpoint information
    endpoint.fromNative(&native_endpoint);
    
    // Return bytes read
    return bytes_read;
}
