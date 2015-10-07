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

#ifndef _NETWORK_TCP_SOCKET_HPP_
#define _NETWORK_TCP_SOCKET_HPP_

#include "../buffer.hpp"
#include "../socket.hpp"
#include "../ip/address.hpp"
#include "../ip/endpoint.hpp"

namespace network {
namespace tcp {

    class Socket :
        public network::Socket
    {        
        public:            
            int open();
            
            int connect(const char *hostname, int port);
            int connect(const std::string hostname, int port);
            int connect(const ip::Address &address, int port);
            int connect(ip::Endpoint &endpoint);
            int shutdown();
            
            int listen(int max_pending = 10);
            int accept(Socket &client);
        
            int write(Buffer &buffer);
            int write(void *data, size_t size);
            
            int read(Buffer &buffer);
            int read(void *data, size_t max_size);
    };

} // namespace tcp
} // namespace network

#endif // _NETWORK_UDP_SOCKET_HPP_