#include "macro_core.hpp"
extern "C" {
#include "export.hpp"
}

MacroCore g_macro;

void 
start(const char* host, int target_hour, int target_min, int target_sec) {
	std::string host_s(host);
	g_macro.Start(host_s, target_hour, target_min, target_sec);
}



void
get_states(int* p_target_host_rtt_ms, uint64_t* p_server_time_ms) {
	int rtt = -1;
	uint64_t server_time;
	uint64_t next_click_time_ms;
	g_macro.GetStates(rtt, server_time, next_click_time_ms);
	*p_target_host_rtt_ms = rtt;
	*p_server_time_ms = server_time;
}
