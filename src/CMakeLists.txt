file(GLOB_RECURSE PANEL_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    /usr/include
    /usr/include/Wt
    /usr/local/Wt/include
    /usr/local/wt/include
    /usr/local/Wt
    /usr/local/wt
    /usr/local/include/jwsmtp
    /usr/local/include/jwsmtp-1.32
    /usr/local/include
    /usr/local/include/wt
    /usr/local/include/Wt
)

add_executable(panel.wt ${PANEL_SRCS})

target_link_libraries(panel.wt
    wt
    wtfcgi
    mysqlclient
    jwsmtp
    curl
)
