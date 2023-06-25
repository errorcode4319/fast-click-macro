#include "macro_core.hpp"
#include "time.hpp"
#include "network.hpp"
#include "mouse.hpp"

MacroCore::MacroCore()
{
}

MacroCore::~MacroCore()
{
}

bool MacroCore::Start(const std::string& target_host, 
	int click_hour, int click_min, int click_sec)
{
	this->Stop();
	m_target_host = target_host;
	m_target_click_hour = click_hour;
	m_target_click_min = click_min;
	m_target_click_sec = click_sec;
	m_is_running = true;
	m_bg_thread = std::make_unique<std::thread>(&MacroCore::process, this);
	return true;
}

void MacroCore::Stop()
{
	m_is_running = false;
	if (m_bg_thread != nullptr) {
		m_bg_thread->join();
		m_bg_thread.reset();
	}
	reset();
}

void MacroCore::process()
{
	bool on_err = false;
	auto ip_addrs = network::GetIpAddrByHostname(m_target_host, on_err);
	int avg_rtt_ms = -1;
	uint64_t last_update = timeutil::GetEpochMs();
	int next_click_time_seconds = getTimeSeconds(
		m_target_click_hour, m_target_click_min, m_target_click_sec);
	int prev_time_seconds = 0;
	while (m_is_running) {

		timeutil::Sleep(200);
		auto cur_epoch = timeutil::GetEpochMs();
		if (cur_epoch - last_update > 5000) {
			avg_rtt_ms = getAverageRTT(ip_addrs);
			last_update = cur_epoch;
		}
		uint64_t server_time_utc = network::GetHttpServerTime(m_target_host);
		auto server_time_tm = timeutil::GetTMFromEpoch(server_time_utc);
		server_time_tm.tm_hour += 9;
		uint64_t server_time_loc = timeutil::GetEpochFromTM(server_time_tm);
		auto server_time_tm_loc = timeutil::GetTMFromEpoch(server_time_loc);

		int server_time_seconds = getTimeSeconds(
			server_time_tm_loc.tm_hour,
			server_time_tm_loc.tm_min,
			server_time_tm_loc.tm_sec
		);
		if (prev_time_seconds != server_time_seconds &&
			next_click_time_seconds == server_time_seconds) {
			prev_time_seconds = server_time_seconds;
			mouse::SendLeftClick();
			std::cout << "!! Mouse Click !!" << std::endl;
		}

		setStates(avg_rtt_ms, server_time_loc, 0);
	}
}

int MacroCore::getAverageRTT(const std::vector<std::string>& ip_addrs) {
	int rtt_total_accum = 0;
	int rtt_valid_count = 0;
	for (auto& ip_addr : ip_addrs) {
		//std::cout << "Get RTT => " << ip_addr << std::endl;
		auto rtt = network::GetIcmpEchoRTT(ip_addr);
		if (rtt < 0) {
			std::cerr << "Failed to get RTT to host: " << ip_addr << std::endl;
		}
		else {
			//std::cout << "RTT: " << rtt << " (" << ip_addr << ")" << std::endl;
			rtt_valid_count += 1;
			rtt_total_accum = rtt;
		}
	}
	if (rtt_valid_count == 0) 
		return -1;	// Failed 
	return rtt_total_accum / rtt_valid_count;
}

void MacroCore::reset() {
	std::scoped_lock lock(m_states_mutex);
	m_target_host = "";
	m_target_click_hour = 0;
	m_target_click_min = 0;
	m_target_click_sec = 0;
	m_target_click_epoch_ms = 0;
	m_target_host_rtt_ms = 0;
	m_server_time_epoch_ms = 0;
	m_current_time_epoch_ms = 0;
	m_next_click_time_ms = 0;
}

void MacroCore::GetStates(
	int& target_host_rtt_ms,
	uint64_t& server_time_ms,
	uint64_t& next_click_time_ms){
	std::scoped_lock lock(m_states_mutex);
	target_host_rtt_ms = m_target_host_rtt_ms;
	server_time_ms = m_server_time_epoch_ms;
	next_click_time_ms = m_next_click_time_ms;
}

void MacroCore::setStates(
	int target_host_rtt_ms,
	uint64_t server_time_ms,
	uint64_t next_click_time_ms) {
	std::scoped_lock lock(m_states_mutex);
	m_target_host_rtt_ms = target_host_rtt_ms;
	m_server_time_epoch_ms = server_time_ms;
	m_next_click_time_ms = next_click_time_ms;
}
