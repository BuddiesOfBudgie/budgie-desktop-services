#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QObject>

#include "config/display.hpp"
#include "dbus/ConfigService.hpp"
#include "outputs/configuration.hpp"
#include "outputs/state.hpp"
#include "outputs/types.hpp"

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
  // Register meta types
  qDBusRegisterMetaType<bd::Outputs::NestedKvMap>();

  qSetMessagePattern("[%{type}] %{if-debug}[%{file}:%{line} %{function}]%{endif}%{message}");
  if (!QDBusConnection::sessionBus().isConnected()) {
    qCritical() << "Cannot connect to the session bus";
    return EXIT_FAILURE;
  }

  bd::DisplayConfig::instance().parseConfig();
  bd::DisplayConfig::instance().debugOutput();
  auto& orchestrator = bd::Outputs::State::instance();

  app.connect(&orchestrator, &bd::Outputs::State::orchestratorInitFailed, [](const QString& error) {
    qFatal() << "Failed to initialize Wayland Orchestrator: " << error;
  });

  app.connect(&orchestrator, &bd::Outputs::State::ready, &bd::DisplayConfig::instance(), &bd::DisplayConfig::apply);

  bd::ConfigService configService;

  orchestrator.init();

  wl_display_roundtrip(bd::Outputs::State::instance().getDisplay());

  return app.exec();
}
