#include "targetstate.hpp"
#include "utils.hpp"

namespace bd::Outputs::Config {
    TargetState::TargetState(QString serial, QObject *parent) : QObject(parent),
        m_serial(serial), m_on(false), m_dimensions(QSize(0, 0)), m_refresh(0), m_mirrorOf(""), m_relative(""), m_horizontal_anchor(HorizontalAnchor::None),
        m_vertical_anchor(VerticalAnchor::None), m_primary(false), m_position(QPoint(0, 0)), m_scale(1.0), m_transform(0), m_adaptive_sync(0) {
    }

    QString TargetState::getSerial() const {
        return m_serial;
    }

    bool TargetState::isOn() const {
        return m_on;
    }
    
    QSize TargetState::getDimensions() const {
        return m_dimensions;
    }

    qulonglong TargetState::getRefresh() const {
        return m_refresh;
    }

    QString TargetState::getRelative() const {
        return m_relative;
    }
    
    HorizontalAnchor::Type TargetState::getHorizontalAnchor() const {
        return m_horizontal_anchor;
    }

    VerticalAnchor::Type TargetState::getVerticalAnchor() const {
        return m_vertical_anchor;
    }

    QPoint TargetState::getPosition() const {
        return m_position;
    }

    bool TargetState::isMirroring() const {
        return !m_mirrorOf.isEmpty();
    }

    bool TargetState::isPrimary() const {
        return m_primary;
    }

    qreal TargetState::getScale() const {
        return m_scale;
    }

    quint8 TargetState::getTransform() const {
        return m_transform;
    }

    QSize TargetState::getResultingDimensions() const {
        return m_resulting_dimensions;
    }

    uint32_t TargetState::getAdaptiveSync() const {
        return m_adaptive_sync;
    }

    void TargetState::setDefaultValues(QSharedPointer<bd::Outputs::Wlr::MetaHead> head) {
        if (head.isNull()) return;
        qDebug() << "TargetState::setDefaultValues" << m_serial;
        auto headData = head.data();
        m_on = headData->isEnabled();

        auto modePtr = headData->getCurrentMode();
        if (!modePtr.isNull()) {
            auto mode = modePtr.data();

            auto dimensionsOpt = mode->getSize();
            if (dimensionsOpt.has_value()) {
                m_dimensions = QSize(dimensionsOpt.value());
            }

            auto refreshOpt = mode->getRefresh();
            if (refreshOpt.has_value()) {
                m_refresh = static_cast<qulonglong>(refreshOpt.value());
            }

            qDebug() << "dimensions" << m_dimensions << "refresh" << m_refresh;
        }

        auto position = headData->getPosition();
        m_position = QPoint(position);
        qDebug() << "position" << m_position;
        auto scale = headData->getScale();
        m_scale = scale;

        auto transform = headData->getTransform();
        m_transform = static_cast<quint8>(transform);

        auto adaptiveSync = headData->getAdaptiveSync();
        m_adaptive_sync = static_cast<uint32_t>(adaptiveSync);

        // Default anchoring from meta head if present (user or config provided)
        m_relative = headData->getRelativeOutput();
        m_horizontal_anchor = headData->getHorizontalAnchor();
        m_vertical_anchor = headData->getVerticalAnchor();
        m_primary = headData->isPrimary();

        qDebug() << "horizontalAnchor" << bd::Outputs::Config::HorizontalAnchor::toString(m_horizontal_anchor) << "\n" << "verticalAnchor" << bd::Outputs::Config::VerticalAnchor::toString(m_vertical_anchor) << "\n" << "primary" << m_primary;
    }

    void TargetState::setOn(bool on) {
        qDebug() << "TargetState::setOn" << m_serial << on;
        m_on = on;
    }

    void TargetState::setDimensions(QSize dimensions) {
        qDebug() << "TargetState::setDimensions" << m_serial << dimensions;
        m_dimensions = dimensions;
    }

    void TargetState::setRefresh(qulonglong refresh) {
        qDebug() << "TargetState::setRefresh" << m_serial << refresh;
        m_refresh = refresh;
    }

    void TargetState::setMirrorOf(const QString& mirrorOf) {
        qDebug() << "TargetState::setMirrorOf" << m_serial << mirrorOf;
        m_mirrorOf = mirrorOf;
        if (!m_mirrorOf.isEmpty()) {
            // If mirroring, unset any explicit relative target
            if (!m_relative.isEmpty()) {
                qDebug() << "Clearing relative due to mirrorOf being set for" << m_serial;
            }
            m_relative.clear();
        }
    }

    void TargetState::setRelative(const QString& relative) {
        qDebug() << "TargetState::setRelative" << m_serial << relative;
        m_relative = relative;
        if (!m_relative.isEmpty()) {
            // If relative is set, unset any mirror target
            if (!m_mirrorOf.isEmpty()) {
                qDebug() << "Clearing mirrorOf due to relative being set for" << m_serial;
            }
            m_mirrorOf.clear();
        }
    }
    
    void TargetState::setHorizontalAnchor(HorizontalAnchor::Type horizontal_anchor) {
        qDebug() << "TargetState::setHorizontalAnchor" << m_serial << bd::Outputs::Config::HorizontalAnchor::toString(horizontal_anchor);
        m_horizontal_anchor = horizontal_anchor;
    }

    void TargetState::setVerticalAnchor(VerticalAnchor::Type vertical_anchor) {
        qDebug() << "TargetState::setVerticalAnchor" << m_serial << bd::Outputs::Config::VerticalAnchor::toString(vertical_anchor);
        m_vertical_anchor = vertical_anchor;
    }

    void TargetState::setPosition(QPoint position) {
        qDebug() << "TargetState::setPosition" << m_serial << position;
        m_position = position;
    }

    void TargetState::setPrimary(bool primary) {
        qDebug() << "TargetState::setPrimary" << m_serial << primary;
        m_primary = primary;
    }

    void TargetState::setScale(qreal scale) {
        qDebug() << "TargetState::setScale" << m_serial << scale;
        m_scale = scale;
    }

    void TargetState::setTransform(quint8 transform) {
        qDebug() << "TargetState::setTransform" << m_serial << transform;
        m_transform = transform;
    }

    void TargetState::setAdaptiveSync(uint32_t adaptiveSync) {
        qDebug() << "TargetState::setAdaptiveSync" << m_serial << adaptiveSync;
        m_adaptive_sync = adaptiveSync;
    }

    void TargetState::updateResultingDimensions() {
        // TODO: See if this adjustment is even needed
        m_resulting_dimensions = (m_scale == 1.0) ? m_dimensions : m_dimensions * m_scale;

        if (m_transform == 0 || m_transform == 180) {
            m_resulting_dimensions.setWidth(m_dimensions.width());
            m_resulting_dimensions.setHeight(m_dimensions.height());
        } else if (m_transform == 90 || m_transform == 270) {
            m_resulting_dimensions.setWidth(m_dimensions.height());
            m_resulting_dimensions.setHeight(m_dimensions.width());
        }
    }
}