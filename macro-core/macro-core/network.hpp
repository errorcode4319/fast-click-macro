/*****************************************************************//**
 * \file   network.hpp
 * \brief  Network Utility Header
 * 
 * \author sdk
 * \date   June 2023
 *********************************************************************/

#pragma once 
#include <iostream>
#include <vector>

namespace network {
	
bool Init();

std::vector<std::string> GetIpAddrByHostname(const std::string& hostname, bool& on_error);


int SendIcmpEchoTest(const std::string& ip);

int GetIcmpEchoRTT(const std::string& ip);

uint64_t GetHttpServerTime(const std::string& host);

}	// namespace network 