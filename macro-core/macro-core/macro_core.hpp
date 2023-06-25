/*****************************************************************//**
 * \file   macro_core.hpp
 * \brief  Marco Core 
 * 
 * \author sdk
 * \date   June 2023
 *********************************************************************/


#pragma once 

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

class MacroCore {

public:
	MacroCore();
	~MacroCore();

	bool Start(const std::string& host, int click_hour, int click_min, int click_sec);

	
	void GetStates(
		int& target_host_rtt_ms, 
		uint64_t& server_time_ms, 
		uint64_t& next_click_time_ms);
	

	void Stop();

private:
	void process();

	void reset();

	void setStates(
		int target_host_rtt_ms,
		uint64_t server_time_ms,
		uint64_t next_click_time_ms);
	
	int getAverageRTT(const std::vector<std::string>& ip_addrs);

	inline int getTimeSeconds(int hour, int min, int sec) {
		return (hour * 60 * 60) + (min * 60) + sec;
	}

private:
	std::string m_target_host;
	int			m_target_click_hour;
	int			m_target_click_min;
	int			m_target_click_sec;
	uint64_t	m_target_click_epoch_ms;

	// States 
	int			m_target_host_rtt_ms;
	uint64_t	m_server_time_epoch_ms;
	uint64_t	m_current_time_epoch_ms;
	uint64_t	m_next_click_time_ms;
	bool		m_has_clicked = false;
	
	bool		m_is_running = false;
	std::unique_ptr<std::thread> m_bg_thread;
	std::mutex	m_states_mutex;
};