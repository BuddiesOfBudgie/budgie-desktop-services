#include <QDBusConnection>
#include <QDBusMetaType>
#include <QGuiApplication>
#include <QObject>

#include "config/outputs/state.hpp"
#include "dbus/ConfigService.hpp"
#include "outputs/state.hpp"
#include "outputs/types.hpp"

int main(int argc, char* argv[]) {
  QGuiApplication app(argc, argv);
  // Register meta types
  qDBusRegisterMetaType<bd::Outputs::NestedKvMap>();
  qDBusRegisterMetaType<bd::Outputs::OutputModeInfo>();
  qDBusRegisterMetaType<bd::Outputs::OutputModesMap>();

  qSetMessagePattern("[%{type}] %{if-debug}[%{file}:%{line} %{function}]%{endif}%{message}");
  if (!QDBusConnection::sessionBus().isConnected()) {
    qCritical() << "Cannot connect to the session bus";
    return EXIT_FAILURE;
  }

  auto& state = bd::Config::Outputs::State::instance();

  state.deserialize();
  auto& orchestrator = bd::Outputs::State::instance();

  app.connect(&orchestrator, &bd::Outputs::State::orchestratorInitFailed, [](const QString& error) {
    qFatal() << "Failed to initialize Wayland Orchestrator: " << error;
  });

  app.connect(&orchestrator, &bd::Outputs::State::ready, &state, &bd::Config::Outputs::State::apply);

  bd::ConfigService configService;

  orchestrator.init();

  return app.exec();
}
