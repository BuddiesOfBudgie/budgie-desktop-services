#pragma once
#include <QObject>

#include "generated/ConfigAdaptorGen.h"

#define OUTPUT_CONFIG_SERVICE_PATH "/org/buddiesofbudgie/Services/Outputs/Config"

namespace bd {
  class ConfigService : public QObject {
      Q_OBJECT
    public:
      explicit ConfigService(QObject* parent = nullptr);
      static ConfigService& instance();
      static ConfigService* create() { return &instance(); }
      ConfigAdaptor*        GetAdaptor();

    public slots:
      void        ResetConfiguration();
      void        SetOutputEnabled(const QString& serial, bool enabled);
      void        SetOutputMode(const QString& serial, int width, int height, qulonglong refreshRate);
      void        SetOutputPositionAnchor(const QString& serial, const QString& relativeSerial, const QString& horizontalAnchor, const QString& verticalAnchor);
      void        SetOutputScale(const QString& serial, double scale);
      void        SetOutputTransform(const QString& serial, quint8 transform);
      void        SetOutputAdaptiveSync(const QString& serial, uint adaptiveSync);
      void        SetOutputPrimary(const QString& serial);
      void        SetOutputMirrorOf(const QString& serial, const QString& mirrorSerial);
      QVariantMap CalculateConfiguration();
      bool        ApplyConfiguration();
      QVariantList GetActions();

    signals:
      void ConfigurationApplied(bool success);

    private:
      ConfigAdaptor* m_adaptor;
  };
}
