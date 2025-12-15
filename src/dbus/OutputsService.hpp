#pragma once
#include "generated/OutputsAdaptorGen.h"

#define OUTPUTS_SERVICE_PATH "/org/buddiesofbudgie/Services/Outputs"

namespace bd {
  class OutputsService : public QObject {
      Q_OBJECT

    public:
      explicit OutputsService(QObject* parent = nullptr);
      static OutputsService& instance();
      static OutputsService* create() { return &instance(); }
      OutputsAdaptor*       GetAdaptor();

    public slots:
      QStringList GetAvailableOutputs();
      QVariantMap GetGlobalRect();
      QString     GetPrimaryOutput();
      QVariantMap GetPrimaryOutputRect();

    private:
      OutputsAdaptor* m_adaptor;
  };
}
