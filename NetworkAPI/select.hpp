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

#ifndef _NETWORK_SELECT_HPP_
#define _NETWORK_SELECT_HPP_

#include <vector>

#include "socket.hpp"

#include "lwip/sockets.h"
#include "lwip/netdb.h"

namespace network
{
    class Select
    {
        public:
            typedef std::vector<Socket *> SocketList;
            
            enum Mode {
                Read      = 0,
                Write     = 1,
                ReadWrite = 2
            };
    
        protected:
            SocketList _write_list;
            SocketList _read_list;
            
            SocketList::iterator _write_list_iterator;
            SocketList::iterator _read_list_iterator;
            
            fd_set _write_set;
            fd_set _read_set;
    
        public:
            ~Select();
        
            int wait(int timeout = 0);
            void clear();
            
            void set(Socket *socket, enum Mode mode);
            void unset(Socket *socket, enum Mode mode);
            
            Socket *getWritable();
            Socket *getReadable();
        
        protected:
            SocketList::iterator _inWriteList(Socket *socket);
            SocketList::iterator _inReadList(Socket *socket);
    };

} // namespace network

#endif // _NETWORK_SELECT_HPP_