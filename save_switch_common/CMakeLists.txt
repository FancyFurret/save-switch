set(TARGET_NAME ${PROJECT_NAME}_common)

add_library(${TARGET_NAME}
        http_client/http_client.cpp
        http_client/http_client.h
        log.cpp
        log.h
        utils.cpp
        utils.h
        save_storage/google_drive/google_drive.cpp
        save_storage/google_drive/google_drive.h
        save_storage/google_drive/google_drive_cache.h
        save_storage/save_storage_entry.h
        save_storage/save_storage.cpp
        save_storage/save_storage.h
        save_storage/save_storage_errors.h
        save_storage/save_storage_errors.h
        save_storage/filepath.cpp
        save_storage/filepath.h
        secrets.h
        http_client/http_response.cpp
        http_client/http_response.h
        terminal.h
        http_client/http_request.cpp
        http_client/http_request.h
        http_client/http_types.h)

find_library(CURL_LIBRARY NAMES curl)
target_link_libraries(${TARGET_NAME} ${CURL_LIBRARY})
target_include_directories(${TARGET_NAME} PUBLIC ${CURL_INCLUDE_DIRS})

find_library(Z_LIBRARY NAMES z)
target_link_libraries(${TARGET_NAME} ${Z_LIBRARY})

target_link_libraries(${TARGET_NAME} nlohmann_json)
target_link_libraries(${TARGET_NAME} fmt)

if (SWITCH)
    target_link_libraries(${TARGET_NAME} ${LIBNX_LIBRARIES})
endif ()
