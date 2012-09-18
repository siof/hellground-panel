file(GLOB_RECURSE PANEL_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${Wt_INCLUDE_DIR}
    ${MYSQL_INCLUDE_DIR}
    ${JWSMTP_INCLUDE_DIR}
)

add_executable(panel.wt ${PANEL_SRCS})

target_link_libraries(panel.wt
    ${Wt_LIBRARY}
    ${Boost_LIBRARIES}
    ${MYSQL_LIBRARY}
    ${JWSMTP_LIBRARY}
)

if (USE_HTTP)
    target_link_libraries(panel.wt ${Wt_HTTP_LIBRARY})
else (USE_HTTP)
    target_link_libraries(panel.wt ${Wt_FCGI_LIBRARY})
endif (USE_HTTP)

install(TARGETS panel.wt
    RUNTIME DESTINATION .
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                GROUP_EXECUTE GROUP_READ
                WORLD_EXECUTE WORLD_READ
)
