#include "state.hpp"

#include <KWayland/Client/registry.h>
#include <QDBusConnection>
#include <QMap>

#include <cstring>

#include "outputs/config/model.hpp"
#include "outputs/wlr/metahead.hpp"
#include "outputs/wlr/metamode.hpp"

namespace bd::Outputs {
  State::State(QObject* parent)
      : QObject(parent), m_registry(nullptr), m_display(nullptr), m_manager(nullptr), m_has_serial(false), m_serial(0), m_has_initted(false),
        m_cached_primary_output(QString()), m_cached_global_rect(QVariantMap()), m_cached_primary_output_rect(QVariantMap()) {}

  State& State::instance() {
    static State _instance(nullptr);
    return _instance;
  }

  void State::init() {
    auto display = wl_display_connect(nullptr);
    if (display == nullptr) {
      emit orchestratorInitFailed(QString("Failed to connect to the Wayland display"));
      return;
    }

    m_display = display;

    m_registry = new KWayland::Client::Registry();
    m_registry->create(m_display);  // Create using our existing display connection

    if (!m_registry->isValid()) {
      wl_display_disconnect(m_display);
      m_registry->release();
      emit orchestratorInitFailed(QString("Failed to create our KWayland registry and manage it"));
      return;
    }

    connect(m_registry, &KWayland::Client::Registry::interfaceAnnounced, this, [this](const QByteArray& interface, quint32 name, quint32 version) {
      if (std::strcmp(interface, QtWayland::zwlr_output_manager_v1::interface()->name) == 0) {
        auto manager = new bd::Outputs::Wlr::OutputManager(nullptr, m_registry, name, QtWayland::zwlr_output_manager_v1::interface()->version);
        connect(manager, &Wlr::OutputManager::done, this, &State::outputManagerDone);
        connect(manager, &Wlr::OutputManager::headAdded, this, &State::onHeadAdded);
        connect(manager, &Wlr::OutputManager::headRemoved, this, &State::onHeadRemoved);
        m_manager = QSharedPointer<Wlr::OutputManager>(manager);
      }
    });

    m_registry->setup();

    if (wl_display_roundtrip(m_display) < 0) {
      emit orchestratorInitFailed(QString("Failed to perform roundtrip on Wayland display"));
      return;
    }

    wl_display_dispatch(m_display);
  }

  QSharedPointer<Wlr::OutputManager> State::getManager() {
    return m_manager;
  }

  wl_display* State::getDisplay() {
    return m_display;
  }

  KWayland::Client::Registry* State::getRegistry() {
    return m_registry;
  }

  bool State::hasSerial() {
    return m_has_serial;
  }

  int State::getSerial() {
    return m_serial;
  }

  QStringList State::AvailableOutputs() const {
    auto outputs = QStringList {};
    if (!m_manager) return outputs;
    for (const auto& output : m_manager->getHeads()) {
      if (output) outputs.append(output->Serial());
    }
    return outputs;
  }

  static QSharedPointer<bd::Outputs::Wlr::MetaHead> getPrimaryOrFirstHead() {
    auto manager = bd::Outputs::State::instance().getManager();
    if (!manager) return nullptr;
    const auto heads = manager->getHeads();
    if (heads.isEmpty()) return nullptr;

    for (const auto& head : heads) {
      if (head && head->Primary()) return head;
    }
    return heads.first();
  }

  QString State::PrimaryOutput() const {
    auto head = getPrimaryOrFirstHead();
    if (!head) return QString();
    return head->Serial();
  }

  QVariantMap State::PrimaryOutputRect() const {
    QVariantMap rect;
    auto        head = getPrimaryOrFirstHead();
    if (!head) return rect;

    // Populate QRect-like map similar to GetModeInfo pattern
    int  x    = head->X();
    int  y    = head->Y();
    int  w    = 0;
    int  h    = 0;
    auto mode = head->getCurrentMode();
    if (mode) {
      auto sizeOpt = mode->getSize();
      if (sizeOpt.has_value()) {
        w = sizeOpt->width();
        h = sizeOpt->height();
      }
    }

    rect["X"]      = x;
    rect["Y"]      = y;
    rect["Width"]  = w;
    rect["Height"] = h;
    return rect;
  }

  QVariantMap State::GlobalRect() const {
    QVariantMap rect;
    auto        calculationResult = bd::Outputs::Config::Model::instance().getCalculationResult();
    if (!calculationResult) return rect;

    auto globalSpace = calculationResult->getGlobalSpace();
    if (!globalSpace) return rect;

    rect["X"]      = globalSpace->x();
    rect["Y"]      = globalSpace->y();
    rect["Width"]  = globalSpace->width();
    rect["Height"] = globalSpace->height();
    return rect;
  }

  void State::registerDbusService() {
    const QString OUTPUTS_SERVICE_PATH = "/org/buddiesofbudgie/Services/Outputs";
    qInfo() << "Registering DBus object at path" << OUTPUTS_SERVICE_PATH;
    if (!QDBusConnection::sessionBus().registerObject(OUTPUTS_SERVICE_PATH, this, QDBusConnection::ExportAllContents)) {
      qCritical() << "Failed to register DBus object at path" << OUTPUTS_SERVICE_PATH;
    }
  }

  void State::outputManagerDone() {
    if (!m_has_initted) {
      // First initialization - register D-Bus service and all output/mode objects
      qInfo() << "Wayland Orchestrator ready";
      qInfo() << "Starting Display DBus Service now (outputs/modes)";

      if (!QDBusConnection::sessionBus().registerService("org.buddiesofbudgie.Services")) {
        qCritical() << "Failed to acquire DBus service name org.buddiesofbudgie.Services";
        return;
      }

      qInfo() << "Registering DBus services for outputs and modes";

      // Register the Outputs service (this object)
      registerDbusService();

      // Register all output objects (which will also register their modes) and connect signals
      if (m_manager) {
        QMap<QString, bd::Outputs::Wlr::MetaHead*> m_outputServices;

        for (const auto& output : m_manager->getHeads()) {
          if (!output) continue;

          QString outputId = output->getIdentifier();

          if (m_outputServices.contains(outputId)) continue;

          // Connect to head signals
          connectHeadSignals(output);

          // This will also register all modes for this output
          output->registerDbusService();
          m_outputServices[outputId] = output.data();
        }
      }

      // Initialize cached values
      m_cached_primary_output = getCurrentPrimaryOutput();
      m_cached_global_rect = getCurrentGlobalRect();
      m_cached_primary_output_rect = getCurrentPrimaryOutputRect();

      // Connect to Model's configurationApplied signal to update global rect
      connect(&bd::Outputs::Config::Model::instance(), &bd::Outputs::Config::Model::configurationApplied,
              this, &State::checkAndEmitSignals);

      emit ready();  // Haven't done our first init, emit that we are ready
    }
    m_has_initted = true;
    emit done();
  }

  void State::onHeadAdded(QSharedPointer<Wlr::MetaHead> head) {
    if (!head) return;
    connectHeadSignals(head);
    checkAndEmitSignals();
  }

  void State::onHeadRemoved(QSharedPointer<Wlr::MetaHead> head) {
    if (!head) return;
    disconnectHeadSignals(head);
    checkAndEmitSignals();
  }

  void State::checkAndEmitSignals() {
    // Check available outputs
    emit availableOutputsChanged();

    // Check primary output
    QString currentPrimary = getCurrentPrimaryOutput();
    if (currentPrimary != m_cached_primary_output) {
      m_cached_primary_output = currentPrimary;
      emit primaryOutputChanged();
    }

    // Check primary output rect
    QVariantMap currentPrimaryRect = getCurrentPrimaryOutputRect();
    if (currentPrimaryRect != m_cached_primary_output_rect) {
      m_cached_primary_output_rect = currentPrimaryRect;
      emit primaryOutputRectChanged();
    }

    // Check global rect
    QVariantMap currentGlobalRect = getCurrentGlobalRect();
    if (currentGlobalRect != m_cached_global_rect) {
      m_cached_global_rect = currentGlobalRect;
      emit globalRectChanged();
    }
  }

  void State::connectHeadSignals(QSharedPointer<Wlr::MetaHead> head) {
    if (!head) return;
    connect(head.data(), &Wlr::MetaHead::stateChanged, this, &State::checkAndEmitSignals);
  }

  void State::disconnectHeadSignals(QSharedPointer<Wlr::MetaHead> head) {
    if (!head) return;
    // Disconnect all signals from this head to this object
    disconnect(head.data(), nullptr, this, nullptr);
  }

  QString State::getCurrentPrimaryOutput() const {
    return PrimaryOutput();
  }

  QVariantMap State::getCurrentGlobalRect() const {
    return GlobalRect();
  }

  QVariantMap State::getCurrentPrimaryOutputRect() const {
    return PrimaryOutputRect();
  }
}
