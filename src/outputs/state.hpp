#pragma once

#include <KWayland/Client/registry.h>
#include <wayland-client.h>
#include <wayland-util.h>

#include <QObject>

#include "outputs/wlr/outputmanager.hpp"

namespace bd::Outputs {
  class State;

  class State : public QObject {
      Q_OBJECT

    public:
      State(QObject* parent);
      static State& instance();
      static State* create() { return &instance(); }

      void                               init();
      QSharedPointer<Wlr::OutputManager> getManager();
      wl_display*                        getDisplay();
      KWayland::Client::Registry*        getRegistry();

      bool hasSerial();
      int  getSerial();

    signals:
      void ready();
      void done();
      void orchestratorInitFailed(QString error);

    public slots:
      void outputManagerDone();

    private:
      KWayland::Client::Registry*        m_registry;
      wl_display*                        m_display;
      QSharedPointer<Wlr::OutputManager> m_manager;
      bool                               m_has_initted;
      bool                               m_has_serial;
      int                                m_serial;
  };

}
