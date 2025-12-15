#pragma once
#include <QSharedPointer>

#include "generated/OutputModeAdaptorGen.h"
#include "outputs/wlr/metamode.hpp"

namespace bd {
  class OutputModeService : public QObject {
      Q_OBJECT
      Q_PROPERTY(int Width READ Width)
      Q_PROPERTY(int Height READ Height)
      Q_PROPERTY(qulonglong RefreshRate READ RefreshRate)
      Q_PROPERTY(bool Preferred READ Preferred)
      Q_PROPERTY(bool Current READ Current)
    public:
      OutputModeService(QSharedPointer<bd::Outputs::Wlr::MetaMode> mode, const QString& outputId, QObject* parent = nullptr);
      ~OutputModeService();

      // Property getters
      int        Width() const;
      int        Height() const;
      qulonglong RefreshRate() const;
      bool       Preferred() const;
      bool       Current() const;

      // D-Bus methods
      Q_INVOKABLE QVariantMap GetModeInfo();

    private:
      QSharedPointer<bd::Outputs::Wlr::MetaMode> m_mode;
      OutputModeAdaptor*                         m_adaptor;
      QString                                    m_outputId;
      bool                                       isCurrentMode() const;
  };
}
