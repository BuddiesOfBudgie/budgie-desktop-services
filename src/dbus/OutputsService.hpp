#pragma once

#include <QDBusContext>
#include <QObject>

#define OUTPUTS_SERVICE_PATH "/org/buddiesofbudgie/Services/Outputs"

namespace bd {
  class OutputsService : public QObject, protected QDBusContext {
      Q_OBJECT
      Q_CLASSINFO("D-Bus Interface", "org.buddiesofbudgie.Services.Outputs")
      Q_PROPERTY(QStringList AvailableOutputs READ AvailableOutputs)
      Q_PROPERTY(QVariantMap GlobalRect READ GlobalRect)
      Q_PROPERTY(QString PrimaryOutput READ PrimaryOutput)
      Q_PROPERTY(QVariantMap PrimaryOutputRect READ PrimaryOutputRect)

    public:
      explicit OutputsService(QObject* parent = nullptr);
      ~OutputsService() = default;

      // Property getters
      QStringList AvailableOutputs() const;
      QVariantMap GlobalRect() const;
      QString     PrimaryOutput() const;
      QVariantMap PrimaryOutputRect() const;
  };
}
