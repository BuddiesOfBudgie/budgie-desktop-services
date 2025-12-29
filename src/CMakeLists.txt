# SPDX-FileCopyrightText: Budgie Desktop Developers
#
# SPDX-License-Identifier: MPL-2.0

add_library(WaylandProtocols_xml OBJECT)
set_property(TARGET WaylandProtocols_xml PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(WaylandProtocols_xml PUBLIC Qt::Core Wayland::Client Plasma::KWaylandClient)
set_target_properties(WaylandProtocols_xml PROPERTIES LINKER_LANGUAGE C)

ecm_add_qtwayland_client_protocol(
  WaylandProtocols_xml PRIVATE_CODE PROTOCOL
  protocols/wlr-output-management-unstable-v1.xml BASENAME
  wlr-output-management-unstable-v1)

set(budgie-desktop-services_SRCS
  # Config
  config/display.cpp
  config/display.hpp
  config/format.hpp
  config/utils.cpp
  config/utils.hpp
  # DBus Services
  dbus/DisplaySchemaTypes.hpp
  dbus/ConfigService.cpp
  dbus/ConfigService.hpp
  dbus/OutputsService.cpp
  dbus/OutputsService.hpp
  dbus/OutputModeService.cpp
  dbus/OutputModeService.hpp
  # Batch System
  outputs/config/enums/actiontype.hpp
  outputs/config/enums/anchors.hpp
  outputs/config/action.cpp
  outputs/config/action.hpp
  outputs/config/model.cpp
  outputs/config/model.hpp
  outputs/config/result.cpp
  outputs/config/result.hpp
  outputs/config/targetstate.cpp
  outputs/config/targetstate.hpp
  # Output
  outputs/types.hpp
  # Output (wlroots-specific)
  outputs/wlr/configuration.cpp
  outputs/wlr/configuration.hpp
  outputs/wlr/configurationhead.cpp
  outputs/wlr/configurationhead.hpp
  outputs/wlr/enums.hpp
  outputs/wlr/head.cpp
  outputs/wlr/head.hpp
  outputs/wlr/metahead.cpp
  outputs/wlr/metahead.hpp
  outputs/wlr/metamode.cpp
  outputs/wlr/metamode.hpp
  outputs/wlr/mode.cpp
  outputs/wlr/mode.hpp
  outputs/wlr/outputmanager.cpp
  outputs/wlr/outputmanager.hpp
  # Output State
  outputs/state.cpp
  outputs/state.hpp
  # System
  sys/SysInfo.cpp
  sys/SysInfo.hpp
)

qt_add_dbus_adaptor(budgie-desktop-services_SRCS dbus/schemas/DisplaySchema.Config.xml dbus/ConfigService.hpp bd::ConfigService)
qt_add_dbus_adaptor(budgie-desktop-services_SRCS dbus/schemas/DisplaySchema.Outputs.xml dbus/OutputsService.hpp bd::OutputsService)
qt_add_dbus_adaptor(budgie-desktop-services_SRCS dbus/schemas/DisplaySchema.Output.xml outputs/wlr/metahead.hpp bd::Outputs::Wlr::MetaHead)
qt_add_dbus_adaptor(budgie-desktop-services_SRCS dbus/schemas/DisplaySchema.OutputMode.xml dbus/OutputModeService.hpp bd::OutputModeService)

add_library(
  budgie-desktop-services STATIC
  ${budgie-desktop-services_SRCS}
)

add_executable(budgie-desktop-services-app main.cpp)
target_include_directories(
  budgie-desktop-services-app PRIVATE ${CMAKE_BINARY_DIR}
                               ${CMAKE_CURRENT_SOURCE_DIR}/includes
                               ${CMAKE_CURRENT_BINARY_DIR}/dbus
                               ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(budgie-desktop-services-app PRIVATE budgie-desktop-services)

target_include_directories(
  budgie-desktop-services
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/includes
          ${CMAKE_CURRENT_SOURCE_DIR}/config ${CMAKE_CURRENT_SOURCE_DIR}/dbus
          ${CMAKE_CURRENT_SOURCE_DIR}/displays ${CMAKE_CURRENT_SOURCE_DIR}/sys)

target_link_libraries(
  budgie-desktop-services PUBLIC Qt::Core Qt::DBus Qt::WaylandClient toml11::toml11 Wayland::Client
                          WaylandProtocols_xml)

set_target_properties(
  budgie-desktop-services-app PROPERTIES OUTPUT_NAME
                                  "org.buddiesofbudgie.Services")

install(TARGETS budgie-desktop-services-app ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES dbus/org.buddiesofbudgie.Services.conf
        DESTINATION ${CMAKE_INSTALL_DATADIR}/dbus-1/system.d)


if(INSTALL_DESKTOP_FILE)
  ecm_install_configured_files(
    INPUT data/org.buddiesofbudgie.Services.desktop.in DESTINATION
    ${KDE_INSTALL_AUTOSTARTDIR}
  )
endif()

if(INSTALL_LABWC)
  install(FILES data/labwc/autostart DESTINATION ${KDE_INSTALL_SYSCONFDIR}/labwc)
endif()

if(INSTALL_SERVICE_FILES)
  ecm_install_configured_files(
    INPUT data/org.buddiesofbudgie.Services.service.in DESTINATION
    ${KDE_INSTALL_SYSTEMDUSERUNITDIR})
endif()
