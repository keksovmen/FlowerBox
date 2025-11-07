#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

# COMPONENT_OBJS += \
# 	switches/fb_switch_iface.o \
# 	switches/fb_switch_service.o \
# 	sensors/fb_sensor_service.o \
# 	sensors/fb_sensor_iface.o \
# 	sensors/fb_sensor_keyboard.o \

COMPONENT_SRCDIRS := . \
    box \
    storage \
    servers \
    states \
    states/boot \
    states/work \
    project_sticks/ \
    sensors/ \
    switches/ \



COMPONENT_OBJEXCLUDE := \
    fb_uart.o \
    fb_wrappers.o \
    storage/fb_ram_storage.o \
    servers/fb_server_debug.o \
    servers/fb_server_html.o \
    servers/fb_server_update.o \
	sensors/fb_sensor_aht20.o \
	sensors/fb_sensor_mp3.o \
	sensors/fb_sensor_temperature.o \
	switches/fb_gpio_switch.o \
	switches/fb_heat_switch.o \
	switches/fb_mp3_switch.o \
	switches/fb_rgb_switch.o \
	switches/fb_time_switch.o \

COMPONENT_DEPENDS := wifi_provisioning json


COMPONENT_ADD_INCLUDEDIRS := . \
    include \
    include/box \
    include/sensors \
    include/servers \
    include/states \
    include/storage \
    include/switches \
    include/states/boot \
    include/states/work \
    include/templates \
    include/project_sticks \
	