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

# ============================================================================
# D-Bus Code Generation
# ============================================================================
# Automatically generate D-Bus adaptor code from XML schemas during build
# This replaces the manual 'task qdbus-gen' workflow

find_package(Qt6 COMPONENTS DBus REQUIRED)

# Find qdbusxml2cpp tool
if(TARGET Qt6::qdbusxml2cpp)
    get_target_property(QT_QDBUSXML2CPP_EXECUTABLE Qt6::qdbusxml2cpp LOCATION)
else()
    find_program(QT_QDBUSXML2CPP_EXECUTABLE qdbusxml2cpp HINTS ${Qt6_BINDIR} REQUIRED)
endif()

message(STATUS "Found qdbusxml2cpp: ${QT_QDBUSXML2CPP_EXECUTABLE}")

set(DBUS_SCHEMA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dbus/schemas)

# Use custom commands to generate D-Bus adaptors with qdbusxml2cpp
# This gives us full control over the command-line arguments
set(DBUS_GENERATED_SOURCES)

# Define the generated files
set(DBUS_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/dbus/generated)
file(MAKE_DIRECTORY ${DBUS_GEN_DIR})

# BatchSystem adaptor
set(BATCH_ADAPTOR_H ${DBUS_GEN_DIR}/BatchSystemAdaptorGen.h)
set(BATCH_ADAPTOR_CPP ${DBUS_GEN_DIR}/BatchSystemAdaptorGen.cpp)
add_custom_command(
    OUTPUT ${BATCH_ADAPTOR_H} ${BATCH_ADAPTOR_CPP}
    COMMAND ${QT_QDBUSXML2CPP_EXECUTABLE} -m -a ${DBUS_GEN_DIR}/BatchSystemAdaptorGen -c BatchSystemAdaptor ${DBUS_SCHEMA_DIR}/DisplaySchema.BatchSystem.xml
    DEPENDS ${DBUS_SCHEMA_DIR}/DisplaySchema.BatchSystem.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating BatchSystemAdaptor from DisplaySchema.BatchSystem.xml"
)
list(APPEND DBUS_GENERATED_SOURCES ${BATCH_ADAPTOR_H} ${BATCH_ADAPTOR_CPP})

# Display adaptor (note: class name is DisplaysAdaptor with 's')
set(DISPLAY_ADAPTOR_H ${DBUS_GEN_DIR}/DisplayAdaptorGen.h)
set(DISPLAY_ADAPTOR_CPP ${DBUS_GEN_DIR}/DisplayAdaptorGen.cpp)
add_custom_command(
    OUTPUT ${DISPLAY_ADAPTOR_H} ${DISPLAY_ADAPTOR_CPP}
    COMMAND ${QT_QDBUSXML2CPP_EXECUTABLE} -m -a ${DBUS_GEN_DIR}/DisplayAdaptorGen -c DisplaysAdaptor ${DBUS_SCHEMA_DIR}/DisplaySchema.Displays.xml
    DEPENDS ${DBUS_SCHEMA_DIR}/DisplaySchema.Displays.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating DisplaysAdaptor from DisplaySchema.Displays.xml"
)
list(APPEND DBUS_GENERATED_SOURCES ${DISPLAY_ADAPTOR_H} ${DISPLAY_ADAPTOR_CPP})

# Output adaptor
set(OUTPUT_ADAPTOR_H ${DBUS_GEN_DIR}/OutputAdaptorGen.h)
set(OUTPUT_ADAPTOR_CPP ${DBUS_GEN_DIR}/OutputAdaptorGen.cpp)
add_custom_command(
    OUTPUT ${OUTPUT_ADAPTOR_H} ${OUTPUT_ADAPTOR_CPP}
    COMMAND ${QT_QDBUSXML2CPP_EXECUTABLE} -m -a ${DBUS_GEN_DIR}/OutputAdaptorGen -c OutputAdaptor ${DBUS_SCHEMA_DIR}/DisplaySchema.Output.xml
    DEPENDS ${DBUS_SCHEMA_DIR}/DisplaySchema.Output.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating OutputAdaptor from DisplaySchema.Output.xml"
)
list(APPEND DBUS_GENERATED_SOURCES ${OUTPUT_ADAPTOR_H} ${OUTPUT_ADAPTOR_CPP})

# OutputMode adaptor
set(OUTPUTMODE_ADAPTOR_H ${DBUS_GEN_DIR}/OutputModeAdaptorGen.h)
set(OUTPUTMODE_ADAPTOR_CPP ${DBUS_GEN_DIR}/OutputModeAdaptorGen.cpp)
add_custom_command(
    OUTPUT ${OUTPUTMODE_ADAPTOR_H} ${OUTPUTMODE_ADAPTOR_CPP}
    COMMAND ${QT_QDBUSXML2CPP_EXECUTABLE} -m -a ${DBUS_GEN_DIR}/OutputModeAdaptorGen -c OutputModeAdaptor ${DBUS_SCHEMA_DIR}/DisplaySchema.OutputMode.xml
    DEPENDS ${DBUS_SCHEMA_DIR}/DisplaySchema.OutputMode.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating OutputModeAdaptor from DisplaySchema.OutputMode.xml"
)
list(APPEND DBUS_GENERATED_SOURCES ${OUTPUTMODE_ADAPTOR_H} ${OUTPUTMODE_ADAPTOR_CPP})

message(STATUS "Generating D-Bus adaptors:")
message(STATUS "  - dbus/generated/BatchSystemAdaptorGen from DisplaySchema.BatchSystem.xml")
message(STATUS "  - dbus/generated/DisplayAdaptorGen from DisplaySchema.Displays.xml")
message(STATUS "  - dbus/generated/OutputAdaptorGen from DisplaySchema.Output.xml")
message(STATUS "  - dbus/generated/OutputModeAdaptorGen from DisplaySchema.OutputMode.xml")
message(STATUS "Generated files will be in: ${CMAKE_CURRENT_BINARY_DIR}/dbus/generated")

# ============================================================================
# Main Library Build
# ============================================================================

add_library(
  budgie-desktop-services STATIC
  config/display.cpp
  config/display.hpp
  config/format.hpp
  config/utils.cpp
  config/utils.hpp
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
  ${DBUS_GENERATED_SOURCES}
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
