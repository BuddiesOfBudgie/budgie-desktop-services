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
  config/display.cpp
  config/display.hpp
  config/format.hpp
  config/utils.cpp
  config/utils.hpp
  dbus/DisplaySchemaTypes.hpp
  dbus/BatchSystemService.cpp
  dbus/BatchSystemService.hpp
  dbus/DisplayService.cpp
  dbus/DisplayService.hpp
  dbus/OutputModeService.cpp
  dbus/OutputModeService.hpp
  dbus/OutputService.cpp
  dbus/OutputService.hpp
  displays/batch-system/CalculationResult.cpp
  displays/batch-system/CalculationResult.hpp
  displays/batch-system/ConfigurationAction.cpp
  displays/batch-system/ConfigurationAction.hpp
  displays/batch-system/ConfigurationBatchSystem.cpp
  displays/batch-system/ConfigurationBatchSystem.hpp
  displays/batch-system/enums.hpp
  displays/batch-system/OutputTargetState.cpp
  displays/batch-system/OutputTargetState.hpp
  displays/configuration.cpp
  displays/configuration.hpp
  displays/output-manager/head/enums.hpp
  displays/output-manager/head/WaylandOutputHead.cpp
  displays/output-manager/head/WaylandOutputHead.hpp
  displays/output-manager/head/WaylandOutputMetaHead.cpp
  displays/output-manager/head/WaylandOutputMetaHead.hpp
  displays/output-manager/mode/enums.hpp
  displays/output-manager/mode/WaylandOutputMetaMode.cpp
  displays/output-manager/mode/WaylandOutputMetaMode.hpp
  displays/output-manager/mode/WaylandOutputMode.cpp
  displays/output-manager/mode/WaylandOutputMode.hpp
  displays/output-manager/WaylandOutputManager.cpp
  displays/output-manager/WaylandOutputManager.hpp
  sys/SysInfo.cpp
  sys/SysInfo.hpp
)

# ============================================================================
# D-Bus Code Generation
# ============================================================================
# Automatically generate D-Bus adaptor code from XML schemas during build
# This replaces the manual 'task qdbus-gen' workflow

find_package(Qt6 COMPONENTS DBus REQUIRED)

qt_add_dbus_adaptor(budgie-desktop-services_SRCS dbus/schemas/DisplaySchema.BatchSystem.xml dbus/BatchSystemService.hpp bd::BatchSystemService)
qt_add_dbus_adaptor(budgie-desktop-services_SRCS dbus/schemas/DisplaySchema.Displays.xml dbus/DisplayService.hpp bd::DisplayService)
qt_add_dbus_adaptor(budgie-desktop-services_SRCS dbus/schemas/DisplaySchema.Output.xml dbus/OutputService.hpp bd::OutputService)
qt_add_dbus_adaptor(budgie-desktop-services_SRCS dbus/schemas/DisplaySchema.OutputMode.xml dbus/OutputModeService.hpp bd::OutputModeService)

# ============================================================================
# Main Library Build
# ============================================================================

add_library(
  budgie-desktop-services STATIC
  ${budgie-desktop-services_SRCS}
)

add_executable(budgie-desktop-services-app main.cpp)
target_include_directories(
  budgie-desktop-services-app PRIVATE ${CMAKE_BINARY_DIR}
                               ${CMAKE_CURRENT_SOURCE_DIR}/includes
                               ${CMAKE_CURRENT_BINARY_DIR}
                               ${CMAKE_CURRENT_BINARY_DIR}/dbus)
target_link_libraries(budgie-desktop-services-app PRIVATE budgie-desktop-services)

target_include_directories(
  budgie-desktop-services
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/includes
          ${CMAKE_CURRENT_SOURCE_DIR}/config ${CMAKE_CURRENT_SOURCE_DIR}/dbus
          ${CMAKE_CURRENT_SOURCE_DIR}/displays ${CMAKE_CURRENT_SOURCE_DIR}/sys
          ${CMAKE_CURRENT_BINARY_DIR}
          ${CMAKE_CURRENT_BINARY_DIR}/dbus)

target_link_libraries(
  budgie-desktop-services PUBLIC Qt::Core Qt::DBus Qt::WaylandClient toml11::toml11 Wayland::Client
                          WaylandProtocols_xml)

set_target_properties(
  budgie-desktop-services-app PROPERTIES OUTPUT_NAME
                                  "org.buddiesofbudgie.Services")

install(TARGETS budgie-desktop-services-app ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES dbus/org.buddiesofbudgie.BudgieDaemon.conf
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
