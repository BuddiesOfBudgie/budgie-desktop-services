#pragma once

#include <QObject>
#include <QSharedPointer>
#include <toml.hpp>

#include "outputs/config/enums/anchors.hpp"
#include "outputs/wlr/metahead.hpp"

namespace bd::Config::Outputs {
    class Output : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)

        // Dimensions
        Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
        Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
        // Refresh rate
        Q_PROPERTY(qulonglong refresh READ refresh WRITE setRefresh NOTIFY refreshChanged)
        // Absolute positioning (shim mode)
        Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged)
        Q_PROPERTY(int y READ y WRITE setY NOTIFY yChanged)
        // Relative positioning
        Q_PROPERTY(QString relativeOutput READ relativeOutput WRITE setRelativeOutput NOTIFY relativeOutputChanged)
        Q_PROPERTY(bd::Outputs::Config::HorizontalAnchor::Type horizontalAnchor READ horizontalAnchor WRITE setHorizontalAnchor NOTIFY horizontalAnchorChanged)
        Q_PROPERTY(bd::Outputs::Config::VerticalAnchor::Type verticalAnchor READ verticalAnchor WRITE setVerticalAnchor NOTIFY verticalAnchorChanged)
        // Transform
        Q_PROPERTY(quint16 transform READ transform WRITE setTransform NOTIFY transformChanged)
        // Adaptive Sync
        Q_PROPERTY(uint32_t adaptiveSync READ adaptiveSync WRITE setAdaptiveSync NOTIFY adaptiveSyncChanged)
        // Primary
        Q_PROPERTY(bool primary READ primary WRITE setPrimary NOTIFY primaryChanged)
        // Scale
        Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
        // Disabled
        Q_PROPERTY(bool disabled READ disabled WRITE setDisabled NOTIFY disabledChanged)

        // Property getters
        public:
            QString identifier() const;
            int width();
            int height();
            qulonglong refresh();
            int x();
            int y();
            QString relativeOutput();
            bd::Outputs::Config::HorizontalAnchor::Type horizontalAnchor();
            bd::Outputs::Config::VerticalAnchor::Type verticalAnchor();
            quint16 transform();
            uint32_t adaptiveSync();
            qreal scale();
            bool primary();
            bool disabled();

            // Property setters
            void setAdaptiveSync(uint32_t adaptiveSync);
            void setDisabled(bool disabled);
            void setHeight(int height);
            void setHorizontalAnchor(bd::Outputs::Config::HorizontalAnchor::Type horizontalAnchor);
            void setIdentifier(const QString& identifier);
            void setPrimary(bool primary);
            void setRefresh(qulonglong refresh);
            void setRelativeOutput(const QString& relativeOutput);
            void setScale(qreal scale);
            void setTransform(quint16 transform);
            void setVerticalAnchor(bd::Outputs::Config::VerticalAnchor::Type verticalAnchor);
            void setWidth(int width);
            void setX(int x);
            void setY(int y);

            // Other methods
            toml::ordered_value toToml();
            void updateFromHead();

    Q_SIGNALS:
        void adaptiveSyncChanged(uint32_t adaptiveSync);
        void disabledChanged(bool disabled);
        void heightChanged(int height);
        void horizontalAnchorChanged(bd::Outputs::Config::HorizontalAnchor::Type horizontalAnchor);
        void identifierChanged(const QString& identifier);
        void primaryChanged(bool primary);
        void refreshChanged(qulonglong refresh);
        void relativeOutputChanged(const QString& relativeOutput);
        void scaleChanged(qreal scale);
        void transformChanged(quint16 transform);
        void verticalAnchorChanged(bd::Outputs::Config::VerticalAnchor::Type verticalAnchor);
        void widthChanged(int width);
        void xChanged(int x);
        void yChanged(int y);
    public:
        Output(QObject* parent = nullptr);
        Output(const toml::value& v, QObject* parent = nullptr);
        ~Output() = default;

    private:
        QSharedPointer<bd::Outputs::Wlr::MetaHead> getMetaHead();

        QString m_identifier;
        int m_width;
        int m_height;
        qulonglong m_refresh;
        int m_x;
        int m_y;
        QString m_relativeOutput;
        bd::Outputs::Config::HorizontalAnchor::Type m_horizontalAnchor;
        bd::Outputs::Config::VerticalAnchor::Type m_verticalAnchor;
        quint16 m_transform;
        uint32_t m_adaptiveSync;
        qreal m_scale;
        bool m_primary;
        bool m_disabled;
    };
}