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

#ifndef _NETWORK_IP_ENDPOINT_HPP_
#define _NETWORK_IP_ENDPOINT_HPP_

#include <cstring>

#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include "address.hpp"

namespace network {
namespace ip {

    class Endpoint
    {
        protected:
            Address _address;
            int _port;
            
        public:
            Endpoint();
            explicit Endpoint(const Endpoint &other);
            explicit Endpoint(const Address &address);
            explicit Endpoint(const Address &address, int port);
            
            int setPort(int port);
            int getPort();
            
            void setAddress(const Address &address);
            Address &getAddress();
            
            int toNative(struct sockaddr_in *endpoint);
            int fromNative(struct sockaddr_in *endpoint);
            
            Endpoint &operator=(const Endpoint &other);
    };

} // namespace ip
} // namespace network

#endif // _NETWORK_IP_ENDPOINT_HPP_