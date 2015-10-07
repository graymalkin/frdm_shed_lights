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
 
#include "address.hpp"
using namespace network::ip;
 
Address::Address()
{
    std::memset(&this->_address, 0, sizeof(this->_address));
}

Address::Address(const int address)
{
    this->fromNative(address);
}

Address::Address(const Address &other)
{
    std::memcpy(&this->_address, &other._address, sizeof(this->_address));
}

Address::Address(const char *address)
{
    std::string _address(address);
    if (this->fromString(_address) < 0) {
       std::memset(&this->_address, 0, sizeof(this->_address));
   }
}

Address::Address(const std::string &address)
{
   if (this->fromString(address) < 0) {
       std::memset(&this->_address, 0, sizeof(this->_address));
   }
}

int
Address::fromString(const char *address)
{
    if (address == NULL) {
        return -1;
    }

    std::string _address(address);
    return this->fromString(_address);
}
 
int
Address::fromString(const std::string &address)
{
    // Decode the ASCI string into integer values.
    int result = std::sscanf(address.c_str(), "%3hhu.%3hhu.%3hhu.%3hhu",
        &this->_address[0],
        &this->_address[1],
        &this->_address[2],
        &this->_address[3]);

    // Check if all four fields got set.
    if (result != 4) {
        return -1;
    }
    
    return 0;
}

std::string
Address::toString()
{
    char address[16];
    
    // Encode integer fields into the ASCI string.
    int result = std::sprintf(address, "%u.%u.%u.%u",
        (int)this->_address[0],
        (int)this->_address[1],
        (int)this->_address[2],
        (int)this->_address[3]);

    // Check if atleast 8 and at maximum 15 bytes has been written.
    if (result < 8 || result > 16) {
        return std::string("0.0.0.0");
    }
    
    return std::string(address);
}

int
Address::fromHostname(const char *hostname)
{
    if (hostname == NULL) {
        return -1;
    }
    
    std::string _hostname(hostname);
    return this->fromHostname(_hostname);
}

int
Address::fromHostname(const std::string &hostname)
{
    // Resolve hostname by DNS
    struct hostent *address = lwip_gethostbyname(hostname.c_str());
    if (address == NULL) {
        return -1;
    }
    
    // Check if the address has the correct size
    if (address->h_length != sizeof(this->_address)) {
        return -1;
    }
    
    // Copy network address
    this->_address[0] = address->h_addr_list[0][0];
    this->_address[1] = address->h_addr_list[0][1];
    this->_address[2] = address->h_addr_list[0][2];
    this->_address[3] = address->h_addr_list[0][3];
    
    // Todo: Free hostent structure? I have no idea...
    return 0;
}

void
Address::fromNative(int address)
{
    // Copy address
    std::memcpy(this->_address, &address, sizeof(this->_address));
}

int
Address::toNative()
{
    int address;
    
    // Copy address
    std::memcpy(&address, this->_address, sizeof(address));
    
    return address;
}

bool
Address::isEmpty()
{
    return ((this->_address[0] == 0) &&
            (this->_address[1] == 0) &&
            (this->_address[2] == 0) &&
            (this->_address[3] == 0));
}

Address &
Address::operator=(const Address &other) {
    std::memcpy(&this->_address, &other._address, sizeof(this->_address));
    return (*this);
}

bool
Address::operator==(const Address &other) {
    return (std::memcmp(&this->_address, &other._address, sizeof(this->_address)) == 0);
}

bool
Address::operator!=(const Address &other) {
    return !((*this) == other);
}