#pragma once
#include <QSharedPointer>

#include "generated/OutputAdaptorGen.h"
#include "outputs/wlr/metahead.hpp"

namespace bd {
  class OutputService : public QObject {
      Q_OBJECT
      Q_PROPERTY(QString Serial READ Serial)
      Q_PROPERTY(QString Name READ Name)
      Q_PROPERTY(QString Description READ Description)
      Q_PROPERTY(QString Make READ Make)
      Q_PROPERTY(QString Model READ Model)
      Q_PROPERTY(bool Enabled READ Enabled)
      Q_PROPERTY(int Width READ Width)
      Q_PROPERTY(int Height READ Height)
      Q_PROPERTY(int X READ X)
      Q_PROPERTY(int Y READ Y)
      Q_PROPERTY(double Scale READ Scale)
      Q_PROPERTY(qulonglong RefreshRate READ RefreshRate)
      Q_PROPERTY(quint8 Transform READ Transform)
      Q_PROPERTY(uint AdaptiveSync READ AdaptiveSync)
      Q_PROPERTY(bool Primary READ Primary)
      Q_PROPERTY(QString MirrorOf READ MirrorOf)
      Q_PROPERTY(QString HorizontalAnchor READ HorizontalAnchor)
      Q_PROPERTY(QString VerticalAnchor READ VerticalAnchor)
      Q_PROPERTY(QString RelativeTo READ RelativeTo)
    public:
      OutputService(QSharedPointer<Outputs::Wlr::MetaHead> output, QObject* parent = nullptr);
      ~OutputService();

      // Property getters
      QString    Serial() const;
      QString    Name() const;
      QString    Description() const;
      QString    Make() const;
      QString    Model() const;
      bool       Enabled() const;
      int        Width() const;
      int        Height() const;
      int        X() const;
      int        Y() const;
      double     Scale() const;
      qulonglong RefreshRate() const;
      quint8     Transform() const;
      uint       AdaptiveSync() const;
      bool       Primary() const;
      QString    MirrorOf() const;
      QString    HorizontalAnchor() const;
      QString    VerticalAnchor() const;
      QString    RelativeTo() const;

      // D-Bus methods
      Q_INVOKABLE QStringList GetAvailableModes();
      Q_INVOKABLE QString     GetCurrentMode();

    private:
      QSharedPointer<Outputs::Wlr::MetaHead> m_output;
      OutputAdaptor*                         m_adaptor;
  };
}
