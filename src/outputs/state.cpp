#include "state.hpp"

#include <KWayland/Client/registry.h>

#include <cstring>

namespace bd::Outputs {
  State::State(QObject* parent)
      : QObject(parent), m_registry(nullptr), m_display(nullptr), m_manager(nullptr), m_has_serial(false), m_serial(0), m_has_initted(false) {}

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

  void State::outputManagerDone() {
    if (!m_has_initted) emit ready();  // Haven't done our first init, emit that we are ready
    m_has_initted = true;
    emit done();
  }
}
