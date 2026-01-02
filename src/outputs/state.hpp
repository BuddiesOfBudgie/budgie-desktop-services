#pragma once

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/registry.h>
#include <wayland-client.h>
#include <wayland-util.h>

#include <QDBusContext>
#include <QObject>

#include "outputs/wlr/outputmanager.hpp"

namespace bd::Outputs {
  class State;

  class State : public QObject, protected QDBusContext {
      Q_OBJECT
      Q_CLASSINFO("D-Bus Interface", "org.buddiesofbudgie.Services.Outputs")
      Q_PROPERTY(QStringList availableOutputs READ availableOutputs NOTIFY availableOutputsChanged)
      Q_PROPERTY(QVariantMap globalRect READ globalRect NOTIFY globalRectChanged)
      Q_PROPERTY(QString primaryOutput READ primaryOutput NOTIFY primaryOutputChanged)
      Q_PROPERTY(QVariantMap primaryOutputRect READ primaryOutputRect NOTIFY primaryOutputRectChanged)

    public:
      State(QObject* parent);
      static State& instance();
      static State* create() { return &instance(); }

      void                                init();
      QSharedPointer<Wlr::OutputManager>  getManager();
      wl_display*                         getDisplay();
      KWayland::Client::Registry*         getRegistry();
      KWayland::Client::ConnectionThread* getConnection();

      bool hasSerial();
      int  getSerial();

      // Property getters
      QStringList availableOutputs() const;
      QVariantMap globalRect() const;
      QString     primaryOutput() const;
      QVariantMap primaryOutputRect() const;

      // D-Bus registration
      void registerDbusService();

    signals:
      void ready();
      void done();
      void orchestratorInitFailed(QString error);
      void availableOutputsChanged();
      void globalRectChanged();
      void primaryOutputChanged();
      void primaryOutputRectChanged();

    public Q_SLOTS:
      void outputManagerDone();

    private Q_SLOTS:
      void onHeadAdded(QSharedPointer<Wlr::MetaHead> head);
      void onHeadRemoved(QSharedPointer<Wlr::MetaHead> head);
      void checkAndEmitSignals();

    private:
      void        connectHeadSignals(QSharedPointer<Wlr::MetaHead> head);
      void        disconnectHeadSignals(QSharedPointer<Wlr::MetaHead> head);
      QString     getCurrentPrimaryOutput() const;
      QVariantMap getCurrentGlobalRect() const;
      QVariantMap getCurrentPrimaryOutputRect() const;

      KWayland::Client::ConnectionThread* m_connection;
      KWayland::Client::Registry*         m_registry;
      wl_display*                         m_display;
      QSharedPointer<Wlr::OutputManager>  m_manager;
      bool                                m_has_initted;
      bool                                m_has_serial;
      int                                 m_serial;
      QString                             m_cached_primary_output;
      QVariantMap                         m_cached_global_rect;
      QVariantMap                         m_cached_primary_output_rect;
  };

}
