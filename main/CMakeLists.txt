idf_component_register(
	SRCS 
		"fb_clock.cpp"
		"fb_event_manager.cpp"
		"fb_file_system.cpp"
		"fb_globals.cpp"
		"fb_http_server.cpp"
		"fb_pins.cpp"
		"fb_update.cpp"
		"fb_settings.cpp"
		"fb_wifi.cpp"
		"main.cpp"
		"states/fb_state_manager.cpp"
		"states/boot/fb_state_booted.cpp"
		"states/boot/fb_state_hw_init.cpp"
		"states/boot/fb_state_wifi_init.cpp"
		"states/boot/fb_state_time_init.cpp"
		"states/work/fb_state_work.cpp"
		"states/work/fb_state_wifi_connected.cpp"

    INCLUDE_DIRS
		"include"
		"include/states"
		"include/states/boot"
		"include/states/work"
		)

spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)