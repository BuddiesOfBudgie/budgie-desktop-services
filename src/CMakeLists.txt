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

add_library(
  budgie-desktop-services STATIC
  # Config
  config/display.cpp
  config/display.hpp
  config/format.hpp
  config/utils.cpp
  config/utils.hpp
  # DBus (Generated)
  dbus/generated/BatchSystemAdaptorGen.cpp
  dbus/generated/BatchSystemAdaptorGen.h
  dbus/generated/DisplayAdaptorGen.cpp
  dbus/generated/DisplayAdaptorGen.h
  dbus/generated/OutputAdaptorGen.cpp
  dbus/generated/OutputAdaptorGen.h
  dbus/generated/OutputModeAdaptorGen.cpp
  dbus/generated/OutputModeAdaptorGen.h
  # DBus Services
  dbus/DisplaySchemaTypes.hpp
  dbus/BatchSystemService.cpp
  dbus/BatchSystemService.hpp
  dbus/DisplayObjectManager.cpp
  dbus/DisplayObjectManager.hpp
  dbus/DisplayService.cpp
  dbus/DisplayService.hpp
  dbus/OutputModeService.cpp
  dbus/OutputModeService.hpp
  dbus/OutputService.cpp
  dbus/OutputService.hpp
  # Batch System
  displays/batch-system/enums/configurationactiontype.hpp
  displays/batch-system/enums/configurationanchors.hpp
  displays/batch-system/configurationaction.cpp
  displays/batch-system/configurationaction.hpp
  displays/batch-system/outputbatchsystem.cpp
  displays/batch-system/outputbatchsystem.hpp
  displays/batch-system/result.cpp
  displays/batch-system/result.hpp
  displays/batch-system/targetstate.cpp
  displays/batch-system/targetstate.hpp
  # Output Manager
  displays/output-manager/wlr/head/enums.hpp
  displays/output-manager/wlr/head/head.cpp
  displays/output-manager/wlr/head/head.hpp
  displays/output-manager/wlr/head/metahead.cpp
  displays/output-manager/wlr/head/metahead.hpp
  displays/output-manager/wlr/mode/enums.hpp
  displays/output-manager/wlr/mode/metamode.cpp
  displays/output-manager/wlr/mode/metamode.hpp
  displays/output-manager/wlr/mode/mode.cpp
  displays/output-manager/wlr/mode/mode.hpp
  displays/output-manager/WaylandOutputManager.cpp
  displays/output-manager/WaylandOutputManager.hpp
  # System
  sys/SysInfo.cpp
  sys/SysInfo.hpp
)

add_executable(budgie-desktop-services-app main.cpp)
target_include_directories(
  budgie-desktop-services-app PRIVATE ${CMAKE_BINARY_DIR}
                               ${CMAKE_CURRENT_SOURCE_DIR}/includes)
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
