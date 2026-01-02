#pragma once

#include <QObject>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QSharedPointer>

#include "outputs/wlr/metahead.hpp"
#include "enums/anchors.hpp"

namespace bd::Outputs::Config {
    class TargetState : public QObject {
        Q_OBJECT

    public:
        TargetState(QString serial, QObject *parent = nullptr);
        ~TargetState() = default;

        QString getSerial() const;
        bool isOn() const;
        QSize getDimensions() const;
        QString getMirrorOf() const;
        qulonglong getRefresh() const;
        QString getRelative() const;
        HorizontalAnchor::Type getHorizontalAnchor() const;
        VerticalAnchor::Type getVerticalAnchor() const;
        QPoint getPosition() const;
        bool isMirroring() const;
        bool isPrimary() const;
        qreal getScale() const;
        quint16 getTransform() const;
        QSize getResultingDimensions() const;
        uint32_t getAdaptiveSync() const;

        void setDefaultValues(QSharedPointer<bd::Outputs::Wlr::MetaHead> head);

        void setOn(bool on);
        void setDimensions(QSize dimensions);
        void setRefresh(qulonglong refresh);
        void setMirrorOf(const QString& mirrorOf);
        void setRelative(const QString& relative);
        void setHorizontalAnchor(HorizontalAnchor::Type horizontal_anchor);
        void setVerticalAnchor(VerticalAnchor::Type vertical_anchor);
        void setPosition(QPoint position);
        void setPrimary(bool primary);
        void setScale(qreal scale);
        void setTransform(quint16 transform);
        void setAdaptiveSync(uint32_t adaptiveSync);

        void updateResultingDimensions();

    private:
        QString m_serial;
        bool m_on;
        QSize m_dimensions;
        QSize m_resulting_dimensions;
        qulonglong m_refresh;
        QString m_mirrorOf;
        QString m_relative;
        HorizontalAnchor::Type m_horizontal_anchor;
        VerticalAnchor::Type m_vertical_anchor;
        bool m_primary;
        QPoint m_position;
        qreal m_scale;
        quint16 m_transform;
        uint32_t m_adaptive_sync;
    };
}