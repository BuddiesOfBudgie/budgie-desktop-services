#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QObject>

#include "config/display.hpp"
#include "dbus/ConfigService.hpp"
#include "dbus/OutputModeService.hpp"
#include "dbus/OutputService.hpp"
#include "dbus/OutputsService.hpp"
#include "outputs/configuration.hpp"
#include "outputs/state.hpp"

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
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

  bd::OutputsService outputsService;
  bd::ConfigService  configService;

  app.connect(&orchestrator, &bd::Outputs::State::ready, &app, []() {
    qInfo() << "Wayland Orchestrator ready";
    qInfo() << "Starting Display DBus Service now (outputs/modes)";

    QMap<QString, bd::OutputService*>     m_outputServices;
    QMap<QString, bd::OutputModeService*> m_modeServices;

    auto manager = bd::Outputs::State::instance().getManager();

    if (!manager) return;

    if (!QDBusConnection::sessionBus().registerService("org.buddiesofbudgie.Services")) {
      qCritical() << "Failed to acquire DBus service name org.buddiesofbudgie.Services";
    }

    qInfo() << "Registering DBus services for outputs and modes";

    for (const auto& output : manager->getHeads()) {
      if (!output) continue;

      QString outputId = output->getIdentifier();

      if (m_outputServices.contains(outputId)) continue;

      auto* outputService        = new bd::OutputService(output);
      m_outputServices[outputId] = outputService;

      for (const auto& mode : output->getModes()) {
        if (!mode) continue;

        QString modeKey = outputId + ":" + mode->getId();

        if (m_modeServices.contains(modeKey)) continue;

        auto* modeService       = new bd::OutputModeService(mode, outputId);
        m_modeServices[modeKey] = modeService;
      }
    }
  });

  orchestrator.init();

  wl_display_roundtrip(bd::Outputs::State::instance().getDisplay());

  return app.exec();
}
