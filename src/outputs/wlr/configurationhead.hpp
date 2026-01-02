#pragma once

#include <QObject>
#include <QSharedPointer>
#include "qwayland-wlr-output-management-unstable-v1.h"

#include "metahead.hpp"

namespace bd::Outputs::Wlr {
    class ConfigurationHead : public QObject, QtWayland::zwlr_output_configuration_head_v1 {
        Q_OBJECT
  
      public:
        ConfigurationHead(bd::Outputs::Wlr::MetaHead* head, ::zwlr_output_configuration_head_v1* config_head, QObject* parent = nullptr);
        bd::Outputs::Wlr::MetaHead* getHead();
        void                   release();
        void                   setAdaptiveSync(uint32_t state);
        void                   setMode(bd::Outputs::Wlr::MetaMode* mode);
        void                   setCustomMode(int32_t width, int32_t height, qulonglong refresh);
        void                   setPosition(int32_t x, int32_t y);
        void                   setTransform(quint16 transform);
        void                   setScale(double scale);
  
      private:
        bd::Outputs::Wlr::MetaHead* m_head;
    };
}