#include "DisplayObjectManager.hpp"

#include "ConfigService.hpp"
#include "OutputsService.hpp"
#include "outputs/state.hpp"

namespace bd {

  DisplayObjectManager& DisplayObjectManager::instance() {
    static DisplayObjectManager mgr;
    return mgr;
  }

  DisplayObjectManager::DisplayObjectManager(QObject* parent) : QObject(parent) {}

  void DisplayObjectManager::onOutputManagerReady() {
    qInfo() << "Wayland Orchestrator ready";
    qInfo() << "Starting Display DBus Service now (outputs/modes)";
    auto manager = Outputs::State::instance().getManager();
    if (!manager) return;

    if (!QDBusConnection::sessionBus().registerService("org.buddiesofbudgie.Services")) {
      qCritical() << "Failed to acquire DBus service name org.buddiesofbudgie.Services";
    }

    for (const auto& output : manager->getHeads()) {
      if (!output) continue;
      QString outputId = output->getIdentifier();
      if (m_outputServices.contains(outputId)) continue;
      auto* outputService        = new OutputService(output, this);
      m_outputServices[outputId] = outputService;
      for (const auto& mode : output->getModes()) {
        if (!mode) continue;
        QString modeKey = outputId + ":" + mode->getId();
        if (m_modeServices.contains(modeKey)) continue;
        auto* modeService       = new OutputModeService(mode, outputId, this);
        m_modeServices[modeKey] = modeService;
      }
    }

    if (!QDBusConnection::sessionBus().registerObject(OUTPUTS_SERVICE_PATH, OutputsService::instance().GetAdaptor(), QDBusConnection::ExportAllContents)) {
      qCritical() << "Failed to register DBus object at path" << OUTPUTS_SERVICE_PATH;
    }

    if (!QDBusConnection::sessionBus().registerObject(
            OUTPUT_CONFIG_SERVICE_PATH, ConfigService::instance().GetAdaptor(), QDBusConnection::ExportAllContents)) {
      qCritical() << "Failed to register DBus object at path" << OUTPUT_CONFIG_SERVICE_PATH;
    }
  }

}  // namespace bd
