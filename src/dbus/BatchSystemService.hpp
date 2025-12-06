#pragma once

#include <QDBusContext>
#include <QObject>

#define BATCH_SYSTEM_SERVICE_PATH "/org/buddiesofbudgie/BudgieDaemon/Displays/BatchSystem"

namespace bd {
  class BatchSystemService : public QObject, protected QDBusContext {
      Q_OBJECT

    public:
      explicit BatchSystemService(QObject* parent = nullptr);
      ~BatchSystemService() = default;

    public Q_SLOTS:
      void         ResetConfiguration();
      void         SetOutputEnabled(const QString& serial, bool enabled);
      void         SetOutputMode(const QString& serial, int width, int height, qulonglong refreshRate);
      void         SetOutputPositionAnchor(const QString& serial, const QString& relativeSerial, int horizontalAnchor, int verticalAnchor);
      void         SetOutputScale(const QString& serial, double scale);
      void         SetOutputTransform(const QString& serial, quint8 transform);
      void         SetOutputAdaptiveSync(const QString& serial, uint adaptiveSync);
      void         SetOutputPrimary(const QString& serial);
      void         SetOutputMirrorOf(const QString& serial, const QString& mirrorSerial);
      QVariantMap  CalculateConfiguration();
      bool         ApplyConfiguration();
      QVariantList GetActions();

    signals:
      void ConfigurationApplied(bool success);
  };
}
