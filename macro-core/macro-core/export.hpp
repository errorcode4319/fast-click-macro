/*****************************************************************//**
 * \file   export.hpp
 * \brief  DLL Export 
 * 
 * \author sdk
 * \date   June 2023
 *********************************************************************/

#include <cstdint>

#ifdef __cplusplus 
extern "C" {
#endif 

__declspec(dllimport) void 
start(const char* host, int target_hour, int target_min, int target_sec);

__declspec(dllimport) void 
get_states(int* p_target_host_rtt_ms, uint64_t* p_server_time_ms);


#ifdef __cplusplus
}
#endif 