file(GLOB_RECURSE PANEL_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${Wt_INCLUDE_DIR}
    ${MYSQL_INCLUDE_DIR}
    ${CURL_INCLUDE_DIR}
    ${JWSMTP_INCLUDE_DIR}
)

add_executable(panel.wt ${PANEL_SRCS})

target_link_libraries(panel.wt
    ${Wt_LIBRARY}
    ${Wt_FCGI_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${MYSQL_LIBRARY}
    ${CURL_LIBRARY}
    ${JWSMTP_LIBRARY}
)

install(TARGETS panel.wt
    RUNTIME DESTINATION ${INSTALL_DIR}
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                GROUP_EXECUTE GROUP_READ
                WORLD_EXECUTE WORLD_READ
)
