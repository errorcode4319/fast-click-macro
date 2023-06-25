#include "network.hpp"
#include "macro_core.hpp"
#include "time.hpp"

int main() {
	network::Init();

	MacroCore macro;
	macro.Start("weverse.io", 0, 57, 00);

	while (true) {
		timeutil::Sleep(500);
		int rtt = -1;
		uint64_t server_time;
		uint64_t next_click_time_ms;
		macro.GetStates(rtt, server_time, next_click_time_ms);
		std::cout << "[State] RTT: " << rtt << "ms | ServerTime: "
			<< timeutil::GetDatetimeFromEpoch(server_time) << std::endl;
	}
}