#include "network.hpp"
#include "time.hpp"
#include "mouse.hpp"
#include <chrono>

int main() {
	network::Init();
	bool on_err = false;
	auto ip_addrs = network::GetIpAddrByHostname("weverse.io", on_err);
	if (on_err) {
		std::cerr << "Failed => on error." << std::endl;
		return 0;
	}
	for (auto& ip_addr : ip_addrs) {
		auto rtt = network::GetIcmpEchoRTT(ip_addr);
		if (rtt < 0) {
			std::cerr << "Failed to get RTT to host: " << ip_addr << std::endl;
		}
		else {
			std::cout << "RTT: " << rtt << " (" << ip_addr << ")" << std::endl;
		}
	}

	while (true) {
		timeutil::Sleep(250);
		uint64_t server_time_utc = network::GetHttpServerTime("weverse.io");
		auto server_time_tm = timeutil::GetTMFromEpoch(server_time_utc);
		server_time_tm.tm_hour += 9;
		uint64_t server_time_loc = timeutil::GetEpochFromTM(server_time_tm);
		uint64_t cur_epoch_ms = timeutil::GetEpochMs();
		std::cout << "ServerTime:\t" << server_time_loc << " => " << timeutil::GetDatetimeFromEpoch(server_time_loc) << std::endl;
		std::cout << "CurrentTime:\t" << cur_epoch_ms << " => " << timeutil::GetDatetimeFromEpoch(cur_epoch_ms) << std::endl;
		mouse::SendLeftClick();
	}


}