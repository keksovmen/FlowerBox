set(_PROJECT MINE)
# set(_PROJECT DIPLOM)

message(STATUS "Selected project: " ${_PROJECT})

if(${_PROJECT} STREQUAL "MINE")
	set(_ADD_SRC 
		"project/fb_project_box_obj.cpp"
		"project/fb_project_hw_obj.cpp"
		"project/fb_project_maper_obj.cpp"
		"project/fb_project_core.cpp"
		"project/fb_project_settings.cpp"
		"sensors/fb_sensor_aht20.cpp"
		)
	set(_ADD_INCLUDES
		"include/project")
else()
	set(_ADD_SRC 
		"project2/fb_project_box_obj.cpp"
		"project2/fb_project_hw_obj.cpp"
		"project2/fb_project_maper_obj.cpp"
		"project2/fb_project_core.cpp"
		"switches/fb_rgb_switch.cpp"
		"switches/fb_mp3_switch.cpp"
		"sensors/fb_sensor_mp3.cpp"
		"df_player/fb_df_player.cpp"
		"fb_uart.cpp"
		)
	set(_ADD_INCLUDES
		"include/project2"
		"include/df_player")
endif()

message(STATUS "ADD SRC: " ${_ADD_SRC})


idf_component_register(
	SRCS 
		${_ADD_SRC}
		"fb_clock.cpp"
		"fb_event_manager.cpp"
		"fb_file_system.cpp"
		"fb_globals.cpp"
		"fb_pins.cpp"
		"fb_update.cpp"
		"fb_settings.cpp"
		"fb_wifi.cpp"
		"fb_service_iface.cpp"
		"fb_provision.cpp"
		"fb_time_scheduler.cpp"
"fb_keyboard_handler.cpp"
"h_keyboard.cpp"
		"main.cpp"
		"box/fb_box.cpp"
		"box/fb_property.cpp"
		"box/fb_sensor.cpp"
		"box/fb_switch.cpp"
		"box/fb_data_entry.cpp"
		"box/fb_object_base.cpp"
		"storage/fb_ram_storage.cpp"
		"storage/fb_nvs_storage.cpp"
		"sensors/fb_sensor_iface.cpp"
		"sensors/fb_sensor_temperature.cpp"
		"sensors/fb_sensor_service.cpp"
		"sensors/fb_sensor_storage.cpp"
		"sensors/fb_sensor_store_service.cpp"
		"sensors/fb_sensor_keyboard.cpp"
		"servers/fb_server.cpp"
		"servers/fb_server_builder.cpp"
		"servers/fb_server_update.cpp"
		"servers/fb_server_html.cpp"
		"servers/fb_server_box.cpp"
		"servers/fb_server_debug.cpp"
		"states/fb_state_manager.cpp"
		"states/fb_main_state_manager.cpp"
		"states/fb_state_sensor_init.cpp"
		"states/boot/fb_state_booted.cpp"
		"states/boot/fb_state_provision.cpp"
		"states/boot/fb_state_hw_init.cpp"
		"states/boot/fb_state_wifi_init.cpp"
		"states/boot/fb_state_time_init.cpp"
		"states/work/fb_state_work.cpp"
		"states/work/fb_state_wifi_connected.cpp"
		"states/update/fb_state_update.cpp"
		"switches/fb_switch_iface.cpp"
		"switches/fb_heat_switch.cpp"
		"switches/fb_time_switch.cpp"
		"switches/fb_switch_service.cpp"
		"templates/fb_templates.cpp"

    INCLUDE_DIRS
		${_ADD_INCLUDES}
		"include"
		"include/box"
		"include/sensors"
		"include/servers"
		"include/states"
		"include/storage"
		"include/switches"
		"include/states/boot"
		"include/states/update"
		"include/states/work"
		"include/templates"
		)

# if(${_PROJECT} STREQUAL "MINE")
# 	target_compile_options(${COMPONENT_LIB} PRIVATE -D_PROJECT=0)
# else()
# 	target_compile_options(${COMPONENT_LIB} PRIVATE -D_PROJECT=1)
# endif()

spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)