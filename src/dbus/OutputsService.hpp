#pragma once

#include <QDBusContext>
#include <QObject>

#define OUTPUTS_SERVICE_PATH "/org/buddiesofbudgie/Services/Outputs"

namespace bd {
  class OutputsService : public QObject, protected QDBusContext {
      Q_OBJECT

    public:
      explicit OutputsService(QObject* parent = nullptr);
      ~OutputsService() = default;

    public Q_SLOTS:
      QStringList GetAvailableOutputs();
      QVariantMap GetGlobalRect();
      QString     GetPrimaryOutput();
      QVariantMap GetPrimaryOutputRect();
  };
}
