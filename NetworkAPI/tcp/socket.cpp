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
using namespace network::tcp;

int
Socket::open()
{
    // Check socket status
    if (this->_status != Socket::Closed) {
        return -1;
    }
    
    // Open socket
    this->_socket = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socket < 0) {
        return -2;
    }
    
    // Update status and return
    this->_status = Socket::Open;
    return 0;
}

int
Socket::connect(const char *hostname, int port)
{
    ip::Address address;
    if (address.fromHostname(hostname) < 0) {
        return -1;
    }
    
    return this->connect(address, port);
}

int
Socket::connect(const std::string hostname, int port)
{
    ip::Address address;
    if (address.fromHostname(hostname) < 0) {
        return -1;
    }
    
    return this->connect(address, port);
}

int
Socket::connect(const ip::Address &address, int port)
{
    ip::Endpoint endpoint(address, port);
    return this->connect(endpoint);
}

int
Socket::connect(ip::Endpoint &endpoint)
{
    // Check socket status
    if ((this->_status != Socket::Open) &&
        (this->_status != Socket::Disconnected))
    {
        return -1;
    }
    
    // Create native endpoint
    struct sockaddr_in native_endpoint;
    endpoint.toNative(&native_endpoint);
    
    // Attempt to connect with remote endpoint.
    int result = lwip_connect(this->_socket,
        (const struct sockaddr *)&native_endpoint, sizeof(native_endpoint));
    
    // Check result
    if (result < 0) {
        return -2;
    }
    
    // Update remote endpoint information.
    this->_remote_endpoint = endpoint;
    
    // Update status and return
    this->_status = Socket::Connected;
    return 0;
}

int
Socket::shutdown()
{
    // Check socket status
    if (this->_status != Socket::Connected) {
        return -1;
    }
    
    // Attempt to shutdown the connection.
    int result = lwip_shutdown(this->_socket, SHUT_RDWR);
    if (result < 0) {
        return -2;
    }
    
    // Update status and return
    this->_status = Socket::Disconnected;
    return 0;
}

int
Socket::listen(int max_pending)
{
    // Check socket status
    if (this->_status != Socket::Open) {
        return -1;
    }
    
    // Put socket into listening mode.
    int result = lwip_listen(this->_socket, max_pending);
    if (result < 0) {
        return -2;
    }
    
    // Update status and return
    this->_status = Socket::Listening;
    return 0;
}

int
Socket::accept(Socket &client)
{
    // Check socket status
    if (this->_status != Socket::Listening) {
        return -1;
    }
    
    // Check client socket status
    if (client._status != Socket::Closed) {
        return -2;
    }
    
    // Create native endpoint
    struct sockaddr_in native_endpoint;
    int native_endpoint_size = sizeof(native_endpoint);
    std::memset(&native_endpoint, 0, sizeof(native_endpoint));
    
    // Accept new (pending) connections.
    int socket = lwip_accept(this->_socket,
        (struct sockaddr*)&native_endpoint, (u32_t *)&native_endpoint_size);
    
    // Did we succeed?
    if (socket < 0) {
        return -3;
    }
    
    // Check if we received the endpoint information correctly.
    if (native_endpoint_size != sizeof(native_endpoint)) {
        printf("Warning: invalid endpoint size received\n\r");
    }
    
    // Populate client socket
    client._socket = socket;
    client._status = Socket::Connected;
    client._remote_endpoint.fromNative(&native_endpoint);
    
    return 0;
}

int
Socket::write(Buffer &buffer)
{
    return this->write(buffer.data(), buffer.length());
}

int
Socket::write(void *data, size_t size)
{
    // Check data buffer and size
    if (data == NULL || size == 0) {
        return -1;
    }

    // Check socket status
    if (this->_status != Socket::Connected) {
        return -2;
    }
    
    // Update status
    this->_status = Socket::Sending;
    
    // Try to send the specified amount of bytes.
    int bytes_written = lwip_send(this->_socket, data, size, 0);
    
    // Update status
    this->_status = (bytes_written == 0)
        ? Socket::Disconnected
        : Socket::Connected;
    
    // Return the result.
    return bytes_written;
}

int
Socket::read(Buffer &buffer)
{
    int result = this->read(buffer.data(), buffer.size());
    if (result >= 0) {
        buffer.length(result);
    }
    
    return result;
}

int
Socket::read(void *data, size_t max_size)
{
    // Check data buffer and size
    if (data == NULL || max_size == 0) {
        return -1;
    }

    // Check socket status
    if (this->_status != Socket::Connected) {
        return -2;
    }
    
    // Update status
    this->_status = Socket::Receiving;
    
    // Try to read data from the socket.
    int bytes_read = lwip_recv(this->_socket, data, max_size, 0);
    
    // Update status
    this->_status = (bytes_read == 0)
        ? Socket::Disconnected
        : Socket::Connected;
            
    // Return bytes read
    return bytes_read;
}
