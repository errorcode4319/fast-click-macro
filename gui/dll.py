from ctypes import *
import ctypes



macro_core_dll = windll.LoadLibrary("./lib/macro-core.dll")

start_func = macro_core_dll["start"]
get_states_func = macro_core_dll["get_states"]
init_func = macro_core_dll["init"]

def init():
    init_func()

def start(target_host: str, click_hour: int, click_min: int, click_sec: int):
    global start_func
    host_b = bytes(target_host, 'utf-8')
    start_func(host_b, click_hour, click_min, click_sec)


def get_states():
    global get_states_func
    rtt = ctypes.c_int()
    server_time = ctypes.c_uint64()
    get_states_func(ctypes.byref(rtt), ctypes.byref(server_time))
    return rtt.value, server_time.value