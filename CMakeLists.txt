project(plasma-pidginrunner)

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

# We add our source code here
set(pidginrunner_SRCS pidginrunner.cpp)

kde4_add_plugin(krunner_pidginrunner ${pidginrunner_SRCS})
target_link_libraries(krunner_pidginrunner
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})

install(TARGETS krunner_pidginrunner
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES pidginrunner.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
