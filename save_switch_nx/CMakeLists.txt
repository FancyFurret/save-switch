set(TARGET_NAME ${PROJECT_NAME}_nx)

include(SwitchTools)

set(NO_ICON true)
add_executable(${TARGET_NAME}
        main.cpp
        config.h
        switch/switch_data.cpp
        switch/switch_data.h
        switch/switch_user.h
        switch/switch_app.h
        switch/switch_save.h
        save_switch.cpp
        save_switch.h)

set_target_properties(${TARGET_NAME} PROPERTIES
        APP_TITLE "SaveSwitch"
        APP_AUTHOR "osum4est"
        APP_VERSION "0.1.0")

add_nro_target(${TARGET_NAME})

target_link_libraries(${TARGET_NAME} ${PROJECT_NAME}_common)
target_link_libraries(${TARGET_NAME} ${LIBNX_LIBRARIES})
