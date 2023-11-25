idf_component_register(
	SRCS 
		"fb_clock.cpp"
		"fb_event_manager.cpp"
		"fb_globals.cpp"
		"fb_pins.cpp"
		"fb_wifi.cpp"
		"main.cpp"
		"states/fb_state_booted.cpp"
		"states/fb_state_time_init.cpp"
		"states/fb_state_manager.cpp"
		"states/fb_state_wifi_connect.cpp"

    INCLUDE_DIRS
		"include"
		"include/states"
		)
