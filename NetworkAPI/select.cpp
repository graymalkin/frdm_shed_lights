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
 
#include "select.hpp"
using namespace network;

Select::~Select()
{
    // Clear fd sets
    FD_ZERO(&this->_read_set);
    FD_ZERO(&this->_write_set);
}
 
int
Select::wait(int timeout)
{
    int max_fd = 0;
    Select::SocketList::iterator entry;
    
    // Clear fd sets
    FD_ZERO(&this->_read_set);
    FD_ZERO(&this->_write_set);
    
    // Create set for writing
    entry = this->_write_list.begin();
    for (; entry != this->_write_list.end(); entry++) {
        int socket = (*entry)->getHandle();
        if (socket == -1) {
            continue;
        }
        
        FD_SET(socket, &this->_write_set);
        max_fd = (max_fd > socket)
            ? max_fd
            : socket;
    }
    
    // Create set for reading
    entry = this->_read_list.begin();
    for (; entry != this->_read_list.end(); entry++) {
        int socket = (*entry)->getHandle();
        if (socket == -1) {
            continue;
        }
        
        FD_SET(socket, &this->_read_set);
        max_fd = (max_fd > socket)
            ? max_fd
            : socket;
    }
    
    // Reset iterators
    this->_write_list_iterator = this->_write_list.begin();
    this->_read_list_iterator = this->_read_list.begin();
    
    // Select on the created sets, wait until timeout.
    if (timeout > 0) {
        struct timeval _timeout;
        _timeout.tv_sec = timeout / 1000;
        _timeout.tv_usec = (timeout - (_timeout.tv_sec * 1000)) * 1000;
        
        return lwip_select(max_fd + 1, &this->_read_set, &this->_write_set, NULL, &_timeout);
    } else {
        // Select on the created sets, wait forever.
        return lwip_select(max_fd + 1, &this->_read_set, &this->_write_set, NULL, NULL);
    }
}

void
Select::clear()
{
    this->_read_list.clear();
    this->_write_list.clear();
}

void
Select::set(Socket *socket, enum Mode mode)
{
    switch (mode) {
        case Select::Read:
            if (this->_inReadList(socket) != this->_read_list.end()) {
                break;
            }
            
            this->_read_list.push_back(socket);
            break;
        
        case Select::Write:
            if (this->_inWriteList(socket) != this->_write_list.end()) {
                break;
            }
            
            this->_write_list.push_back(socket);
            break;
           
        case Select::ReadWrite:
            this->set(socket, Select::Read);
            this->set(socket, Select::Write);
            break;
    }
}

void
Select::unset(Socket *socket, enum Mode mode)
{
    Select::SocketList::iterator entry;

    switch (mode) {
        case Select::Read:
            entry = this->_inReadList(socket);
            if (entry == this->_read_list.end()) {
                break;
            }
            
            this->_read_list.erase(entry);
            break;
        
        case Select::Write:
            entry = this->_inWriteList(socket);
            if (entry == this->_write_list.end()) {
                break;
            }
            
            this->_write_list.erase(entry);
            break;
           
        case Select::ReadWrite:
            this->unset(socket, Select::Read);
            this->unset(socket, Select::Write);
            break;
    }
}

Socket *
Select::getWritable()
{
    if (this->_write_list_iterator == this->_write_list.end()) {
        return NULL;
    }
    
    for (; this->_write_list_iterator != this->_write_list.end(); this->_write_list_iterator++) {
        int socket = (*this->_write_list_iterator)->getHandle();
        if (socket == -1) {
            continue;
        }
        
        if (FD_ISSET(socket, &this->_write_set)) {
            Select::SocketList::iterator result = this->_write_list_iterator;
            this->_write_list_iterator++;
            return (*result);
        }
    }
    
    return NULL;
}

Socket *
Select::getReadable()
{
    if (this->_read_list_iterator == this->_read_list.end()) {
        return NULL;
    }
    
    for (; this->_read_list_iterator != this->_read_list.end(); this->_read_list_iterator++) {
        int socket = (*this->_read_list_iterator)->getHandle();
        if (socket == -1) {
            continue;
        }
        
        if (FD_ISSET(socket, &this->_read_set)) {
            Select::SocketList::iterator result = this->_read_list_iterator;
            this->_read_list_iterator++;
            return (*result);
        }
    }
    
    return NULL;
}

Select::SocketList::iterator
Select::_inWriteList(Socket *socket)
{
    Select::SocketList::iterator entry = this->_write_list.begin();
    for (; entry != this->_write_list.end(); entry++) {
        if ((*entry) == socket) {
            return entry;
        }
    }
    
    return this->_write_list.end();
}

Select::SocketList::iterator
Select::_inReadList(Socket *socket)
{
    Select::SocketList::iterator entry = this->_read_list.begin();
    for (; entry != this->_read_list.end(); entry++) {
        if ((*entry) == socket) {
            return entry;
        }
    }
    
    return this->_read_list.end();
}