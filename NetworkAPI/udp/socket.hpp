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

#ifndef _NETWORK_UDP_SOCKET_HPP_
#define _NETWORK_UDP_SOCKET_HPP_

#include "../buffer.hpp"
#include "../socket.hpp"
#include "../ip/address.hpp"

namespace network {
namespace udp {

    class Socket :
        public network::Socket
    {
        public:        
            int open();
        
            int send(Buffer &buffer, ip::Address &address, int port);
            int send(Buffer &buffer, ip::Endpoint &endpoint);
            int send(void *data, size_t size, ip::Address &address, int port);
            int send(void *data, size_t size, ip::Endpoint &endpoint);
            
            int receive(Buffer &buffer);
            int receive(void *data, size_t max_size);
            int receive(void *data, size_t max_size, ip::Endpoint &endpoint);
    };

} // namespace udp
} // namespace network

#endif // _NETWORK_UDP_SOCKET_HPP_