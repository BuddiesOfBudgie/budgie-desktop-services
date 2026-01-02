#pragma once

#include <QDBusContext>
#include <QObject>

#define OUTPUT_CONFIG_SERVICE_PATH "/org/buddiesofbudgie/Services/Outputs/Config"

namespace bd {
  class ConfigService : public QObject, protected QDBusContext {
      Q_OBJECT
      Q_CLASSINFO("D-Bus Interface", "org.buddiesofbudgie.Services.Config")

    public:
      explicit ConfigService(QObject* parent = nullptr);
      ~ConfigService() = default;

    public Q_SLOTS:
      void        ResetConfiguration();
      void        SetOutputEnabled(const QString& serial, bool enabled);
      void        SetOutputMode(const QString& serial, int width, int height, qulonglong refreshRate);
      void        SetOutputPositionAnchor(const QString& serial, const QString& relativeSerial, const QString& horizontalAnchor, const QString& verticalAnchor);
      void        SetOutputScale(const QString& serial, double scale);
      void        SetOutputTransform(const QString& serial, quint16 transform);
      void        SetOutputAdaptiveSync(const QString& serial, uint adaptiveSync);
      void        SetOutputPrimary(const QString& serial);
      void        SetOutputMirrorOf(const QString& serial, const QString& mirrorSerial);
      QVariantMap CalculateConfiguration();
      bool        ApplyConfiguration();
      QVariantList GetActions();

    Q_SIGNALS:
      void ConfigurationApplied(bool success);
  };
}
