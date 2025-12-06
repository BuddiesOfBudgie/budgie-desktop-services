#pragma once

#include <QDBusContext>
#include <QObject>

#define DISPLAY_SERVICE_NAME "org.buddiesofbudgie.BudgieDaemon.Displays"
#define DISPLAY_SERVICE_PATH "/org/buddiesofbudgie/BudgieDaemon/Displays"

namespace bd {
  class DisplayService : public QObject, protected QDBusContext {
      Q_OBJECT

    public:
      explicit DisplayService(QObject* parent = nullptr);
      ~DisplayService() = default;

    public Q_SLOTS:
      QStringList GetAvailableOutputs();
      QVariantMap GetGlobalRect();
      QString     GetPrimaryOutput();
      QVariantMap GetPrimaryOutputRect();
  };
}
