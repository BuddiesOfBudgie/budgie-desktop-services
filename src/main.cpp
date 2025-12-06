#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QObject>

#include "config/display.hpp"
#include "dbus/BatchSystemService.hpp"
#include "dbus/DisplayService.hpp"
#include "dbus/OutputModeService.hpp"
#include "dbus/OutputService.hpp"
#include "displays/configuration.hpp"
#include "displays/output-manager/WaylandOutputManager.hpp"

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
  qSetMessagePattern("[%{type}] %{if-debug}[%{file}:%{line} %{function}]%{endif}%{message}");
  if (!QDBusConnection::sessionBus().isConnected()) {
    qCritical() << "Cannot connect to the session bus";
    return EXIT_FAILURE;
  }

  qDBusRegisterMetaType<OutputModesList>();
  qDBusRegisterMetaType<OutputDetailsList>();

  bd::DisplayConfig::instance().parseConfig();
  bd::DisplayConfig::instance().debugOutput();
  auto& orchestrator = bd::WaylandOrchestrator::instance();

  app.connect(&orchestrator, &bd::WaylandOrchestrator::orchestratorInitFailed, [](const QString& error) {
    qFatal() << "Failed to initialize Wayland Orchestrator: " << error;
  });

  app.connect(&orchestrator, &bd::WaylandOrchestrator::ready, &bd::DisplayConfig::instance(), &bd::DisplayConfig::apply);

  bd::DisplayService     displayService;
  bd::BatchSystemService batchSystemService;

  app.connect(&orchestrator, &bd::WaylandOrchestrator::ready, &app, []() {
    qInfo() << "Wayland Orchestrator ready";
    qInfo() << "Starting Display DBus Service now (outputs/modes)";

    QMap<QString, bd::OutputService*>     m_outputServices;
    QMap<QString, bd::OutputModeService*> m_modeServices;

    auto manager = bd::WaylandOrchestrator::instance().getManager();
    if (!manager) return;

    if (!QDBusConnection::sessionBus().registerService("org.buddiesofbudgie.BudgieDaemon")) {
      qCritical() << "Failed to acquire DBus service name org.buddiesofbudgie.BudgieDaemon";
    }

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

  wl_display_roundtrip(bd::WaylandOrchestrator::instance().getDisplay());

  return app.exec();
}
