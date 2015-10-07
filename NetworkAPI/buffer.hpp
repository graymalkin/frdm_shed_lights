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

#ifndef _NETWORK_BUFFER_HPP_
#define _NETWORK_BUFFER_HPP_

#include <cstdlib>
#include <string>
#include <cstring>

namespace network
{
    class Buffer
    {
        // Members
        protected:
            void *_memory;
            
            size_t _size;
            size_t _length;
        
        // Methods  
        public:
            Buffer();
            explicit Buffer(const Buffer &other);
            explicit Buffer(const size_t size);
            explicit Buffer(const std::string &other);
        
            ~Buffer();
            
            int read(void *data, size_t max_length, size_t offset = 0);
            int write(const void *data, size_t length, size_t offset = 0);
            int flush();
            
            void *data(size_t offset = 0);
            
            void length(size_t length);
            size_t length();
            
            size_t size();
            size_t free();
            
            Buffer &operator=(const Buffer &other);
            Buffer &operator=(const std::string &message);
       
       protected:
            int _memory_allocate(size_t size);
            void _memory_free();
            
    };

} // namespace network

#endif // _NETWORK_UDP_SOCKET_HPP_