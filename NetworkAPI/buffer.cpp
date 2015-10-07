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

#include "buffer.hpp"
using namespace network;

Buffer::Buffer():
    _memory(NULL), _size(0), _length(0)
{}

Buffer::Buffer(const size_t size):
    _size(size), _length(0)
{
    this->_memory_allocate(size);
}

Buffer::Buffer(const Buffer &other):
    _size(other._size), _length(other._length)
{
    if (this->_memory_allocate(other._size) != -1) {
        std::memcpy(this->_memory, other._memory, other._size);
    }
}

Buffer::Buffer(const std::string &other):
    _size(other.size()), _length(other.length())
{
    if (this->_memory_allocate(other.size()) != -1) {
        std::memcpy(this->_memory, other.data(), other.size());
    }
}

Buffer::~Buffer()
{
    this->_memory_free();
}

Buffer &
Buffer::operator=(const Buffer &other)
{
    // Resize buffer if necessary
    if (this->_size < other._size) {
        this->_memory_free();
        if (this->_memory_allocate(other._size) < 0) {
            return (*this);
        }
    }
    
    // Copy information
    std::memcpy(this->_memory, other._memory, other._size);
    this->_size = other._size;
    this->_length = other._length;
    
    return (*this);
}

Buffer &
Buffer::operator=(const std::string &other)
{
    // Resize buffer if necessary
    if (this->_size < other.size()) {
        this->_memory_free();
        if (this->_memory_allocate(other.size()) < 0) {
            return (*this);
        }
    }
    
    // Copy information
    std::memcpy(this->_memory, other.data(), other.size());
    this->_size = other.size();
    this->_length = other.length();
    
    return (*this);
}

int
Buffer::read(void *data, size_t max_length, size_t offset)
{
    if ((offset >= this->_size) || (this->_memory == NULL)) {
        return 0;
    }
    
    int bytes_to_copy = (max_length > this->_size - offset)
        ? this->_size - offset
        : max_length;
    
    std::memcpy(data, (unsigned char*)this->_memory + offset, bytes_to_copy);
    return bytes_to_copy;
}

int
Buffer::write(const void *data, size_t length, size_t offset)
{
    if ((offset >= this->_size) || (this->_memory == NULL)) {
        return 0;
    }
    
    int bytes_to_copy = (length > this->_size - offset)
        ? this->_size - offset
        : length;
        
    std::memcpy((unsigned char*)this->_memory + offset, data, bytes_to_copy);
    this->_length += bytes_to_copy;
    return bytes_to_copy;
}

int
Buffer::flush()
{
    if (this->_memory == NULL) {
        return -1;
    }

    this->_length = 0;
    std::memset(this->_memory, 0, this->_size);
    
    return 0;
}

void *
Buffer::data(size_t offset)
{
    if (this->_memory == NULL) {
        return NULL;
    }

    if (offset >= this->_size) {
        return NULL;
    }

    return ((unsigned char*)this->_memory + offset);
}

void
Buffer::length(size_t length)
{
    if (length > this->_size) {
        this->_length = this->_size;
    } else {
        this->_length = length;
    }
}

size_t
Buffer::length()
{
    return this->_length;
}

size_t
Buffer::size()
{
    return this->_size;
}

size_t
Buffer::free()
{
    return this->_size - this->_length;
}

int
Buffer::_memory_allocate(size_t size)
{
    this->_memory = calloc(size, sizeof(unsigned char));

    if (this->_memory == NULL) {
        this->_size = 0;
        this->_length = 0;
        return -1;
    }
    
    return 0;
}

void
Buffer::_memory_free()
{
    if (this->_memory != NULL) {
        std::free(this->_memory);
    }
}