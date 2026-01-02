#pragma once

#include <KWayland/Client/output.h>
#include <KWayland/Client/registry.h>
#include <QObject>
#include <QSharedPointer>
#include "qwayland-wlr-output-management-unstable-v1.h"

#include "configuration.hpp"
#include "configurationhead.hpp"
#include "metahead.hpp"


namespace bd::Outputs::Wlr {
    class OutputManager : public QObject, QtWayland::zwlr_output_manager_v1 {
        Q_OBJECT
  
      public:
      OutputManager(QObject* parent, KWayland::Client::Registry* registry, uint32_t serial, uint32_t version);
        //      static WaylandOutputManager& instance();
  
        QSharedPointer<Configuration>            configure();
        QList<QSharedPointer<bd::Outputs::Wlr::MetaHead>>          getHeads();
        QSharedPointer<bd::Outputs::Wlr::MetaHead>                 getOutputHead(const QString& str);
        QList<QSharedPointer<ConfigurationHead>> applyNoOpConfigurationForNonSpecifiedHeads(
            Configuration* config,
            const QStringList&          identifiers);
  
        uint32_t getSerial();
        uint32_t getVersion();

      signals:
        void done();
        void headAdded(QSharedPointer<bd::Outputs::Wlr::MetaHead> head);
        void headRemoved(QSharedPointer<bd::Outputs::Wlr::MetaHead> head);
  
      protected:
        void zwlr_output_manager_v1_head(zwlr_output_head_v1* head) override;
        void zwlr_output_manager_v1_finished() override;
        void zwlr_output_manager_v1_done(uint32_t serial) override;
  
      private:

        KWayland::Client::Registry*                   m_registry;
        QList<QSharedPointer<bd::Outputs::Wlr::MetaHead>> m_heads;
        uint32_t                                      m_serial;
        bool                                          m_has_serial;
        uint32_t                                      m_version;
    };
}