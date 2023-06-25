/*****************************************************************//**
 * \file   mouse.hpp
 * \brief  Mouse SendInput 
 * 
 * \author sdk
 * \date   June 2023
 *********************************************************************/

#include <Windows.h>
#include <cstdint>

namespace mouse {

void SendLeftClick(uint64_t delay_ms = 10);

}
