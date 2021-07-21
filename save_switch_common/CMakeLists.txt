set(TARGET_NAME ${PROJECT_NAME}_common)

add_library(${TARGET_NAME}
        log.cpp
        log.h
        utils.cpp
        utils.h
        secrets.h
        terminal.h
        http_client/http_client.cpp
        http_client/http_client.h
        http_client/http_response.cpp
        http_client/http_response.h
        http_client/http_request.cpp
        http_client/http_request.h
        http_client/http_types.h
        cloud_storage/google_drive/google_drive.cpp
        cloud_storage/google_drive/google_drive.h
        cloud_storage/google_drive/google_drive_cache.h
        cloud_storage/cloud_storage_entry.h
        cloud_storage/cloud_storage.cpp
        cloud_storage/cloud_storage.h
        cloud_storage/cloud_storage_errors.h
        cloud_storage/cloud_storage_errors.h
        cloud_storage/filepath.cpp
        cloud_storage/filepath.h)

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
