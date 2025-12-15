#include <QCryptographicHash>
#include <QtAlgorithms>
#include <optional>

#include "sys/SysInfo.hpp"
#include "metahead.hpp"
#include "head.hpp"

namespace bd::Outputs::Wlr {
    MetaHead::MetaHead(QObject *parent, KWayland::Client::Registry *registry)
            : QObject(parent),
              m_registry(registry),
              m_current_mode(nullptr),
              m_head(nullptr),
              m_position(QPoint{0, 0}),
              m_transform(0),
              m_scale(1.0),
              m_is_available(false),
              m_enabled(false),
              m_adaptive_sync(),
              m_relative_output(""),
              m_horizontal_anchor(bd::Outputs::Config::HorizontalAnchor::None),
              m_vertical_anchor(bd::Outputs::Config::VerticalAnchor::None),
              m_primary(false) {
    }

    MetaHead::~MetaHead() {
        for (auto &mode: m_output_modes) {
            if (!mode) continue;
            auto mode_ptr = mode.data();
            mode_ptr->deleteLater();
        }
        m_output_modes.clear();
    }

    // Getters

    QtWayland::zwlr_output_head_v1::adaptive_sync_state MetaHead::getAdaptiveSync() {
        return m_adaptive_sync;
    }

    QSharedPointer<bd::Outputs::Wlr::MetaMode> MetaHead::getCurrentMode() {
        return m_current_mode;
    }

    QString MetaHead::getDescription() {
        return m_description;
    }

    QSharedPointer<bd::Outputs::Wlr::Head> MetaHead::getHead() {
        return m_head;
    }

    QString MetaHead::getIdentifier() {
        // Have a valid serial, use that as the identifier
        if (!m_serial.isNull() && !m_serial.isEmpty()) {
            return m_serial;
        }

        // Already generated an identifier
        if (!m_identifier.isNull() && !m_identifier.isEmpty()) { return m_identifier; }

        // Default to unique name being machine ID + name
        auto unique_name = QString{SysInfo::instance().getMachineId() + "_" + m_name};

        if (!m_make.isNull() && !m_model.isNull() && !m_make.isEmpty() && !m_model.isEmpty()) {
            unique_name = QString {m_make + " " + m_model + " (" + m_name + ")"};
        }

        auto hash = QCryptographicHash::hash(unique_name.toUtf8(), QCryptographicHash::Md5);

        m_identifier = QString{hash.toHex()};

        return m_identifier;
    }

    QSharedPointer<bd::Outputs::Wlr::MetaMode> MetaHead::getModeForOutputHead(int width, int height, qulonglong refresh) {
        for (const auto& mode_ptr: m_output_modes) {
            if (!mode_ptr) continue;

            auto mode = mode_ptr.data();
            auto modeSizeOpt = mode->getSize();
            auto modeRefreshOpt = mode->getRefresh();
            if (!modeSizeOpt.has_value() || !modeRefreshOpt.has_value()) continue;
            if (!modeSizeOpt.value().isValid()) continue;
            auto modeSize = modeSizeOpt.value();
            auto modeRefresh = static_cast<qulonglong>(modeRefreshOpt.value());

            if (modeSize.width() == width && modeSize.height() == height && modeRefresh == refresh) { return mode_ptr; }
        }

        return nullptr;
    }

    QList<QSharedPointer<bd::Outputs::Wlr::MetaMode>> MetaHead::getModes() {
        return m_output_modes;
    }

    QString MetaHead::getMake() {
        return m_make;
    }

    QString MetaHead::getModel() {
        return m_model;
    }

    QString MetaHead::getName() {
        return m_name;
    }

    QPoint MetaHead::getPosition() {
        return m_position;
    }

    double MetaHead::getScale() {
        return m_scale;
    }

    int MetaHead::getTransform() {
        return m_transform;
    }

    std::optional<::zwlr_output_head_v1*> MetaHead::getWlrHead() {
        if (!m_head) return std::nullopt;
        auto head = m_head.data()->getWlrHead();
        if (head == nullptr) return std::nullopt;
        return std::make_optional(head);
    }

    bool MetaHead::isAvailable() {
        return m_is_available;
    }

    bool MetaHead::isBuiltIn() {
        // Generate identifier if necessary
        getIdentifier();
        // Return if identifier exists
        return !m_identifier.isNull() && !m_identifier.isEmpty();
    }

    bool MetaHead::isEnabled() {
        return m_enabled;
    }

    bool MetaHead::isPrimary() {
        return m_primary;
    }

    // Setters

    void MetaHead::setHead(::zwlr_output_head_v1 *wlr_head) {
        auto head = new bd::Outputs::Wlr::Head(this, wlr_head);
        m_head = QSharedPointer<bd::Outputs::Wlr::Head>(head);
        m_is_available = true;
        emit headAvailable();
        connect(head, &bd::Outputs::Wlr::Head::headFinished, this, &MetaHead::headDisconnected);
        connect(head, &bd::Outputs::Wlr::Head::modeAdded, this, &MetaHead::addMode);
        connect(head, &bd::Outputs::Wlr::Head::modeChanged, this, &MetaHead::currentModeChanged);
        connect(head, &bd::Outputs::Wlr::Head::propertyChanged, this, &MetaHead::setProperty);
    }

    void MetaHead::setPosition(QPoint position) {
        if (position.isNull()) {
            qWarning() << "Invalid position provided, not setting position on head: " << getIdentifier();
            return;
        }

        if (m_position == position) {
            qWarning() << "Position is already set to" << position.x() << position.y() << ", not changing it.";
            return;
        }

        qDebug() << "Setting position on head" << getIdentifier() << "to" << m_position.x() << m_position.y();
        m_position.setX(position.x());
        m_position.setY(position.y());
        emit propertyChanged(MetaHeadProperty::Property::Position, QVariant{m_position});
    }

    void MetaHead::setPrimary(bool primary) {
        if (m_primary == primary) return;
        m_primary = primary;
    }

    void MetaHead::unsetModes() {
        qDebug() << "Unsetting modes for head: " << getIdentifier();
        for (const auto& mode_ptr: m_output_modes) {
            if (!mode_ptr) continue;
            auto mode = mode_ptr.data();
            mode->unsetMode(); // Unset the mode so it no longer holds a reference to a zwlr_output_mode_v1 (WaylandOutputMode)
        }
    }

    // Slots

    QSharedPointer<bd::Outputs::Wlr::MetaMode> MetaHead::addMode(::zwlr_output_mode_v1 *mode) {
        auto output_mode = new bd::Outputs::Wlr::MetaMode(this, mode);
        auto shared_ptr = QSharedPointer<bd::Outputs::Wlr::MetaMode>(output_mode);

        connect(output_mode, &bd::Outputs::Wlr::MetaMode::done, this, [this, output_mode, shared_ptr]() {
            // Check if this already exists
            for (const auto &mode_ptr: m_output_modes) {
                if (!mode_ptr) continue;
                auto existing_mode = mode_ptr.data();
                // Already exists, set the wlr_mode of the existing mode and delete this newly created meta mode
                if (existing_mode->isSameAs(output_mode)) {
                    qDebug() << "Found an output mode that matches one we already have, deleting the new one.";
                    auto wlr_mode_opt = output_mode->getWlrMode();
                    if (wlr_mode_opt.has_value() && wlr_mode_opt.value() != nullptr) {
                        qDebug() << "Setting existing mode to the new wlr_mode.";
                        existing_mode->setMode(const_cast<::zwlr_output_mode_v1*>(wlr_mode_opt.value()));
                    }
//                    shared_ptr.clear();
                    return;
                }
            }

            // Doesn't already exist, add it
            qDebug() << "Adding new output mode to head: " << getIdentifier() << " with size: "
                     << output_mode->getSize().value_or(QSize(0, 0))
                     << " and refresh: " << static_cast<qulonglong>(output_mode->getRefresh().value_or(0));
            m_output_modes.append(shared_ptr);
        });

        return shared_ptr;
    }

    void MetaHead::currentModeChanged(::zwlr_output_mode_v1 *mode) {
        qDebug() << "Current mode changed for output: " << getIdentifier();
        for (const auto &output_mode_ptr: m_output_modes) {
            if (!output_mode_ptr || output_mode_ptr.isNull()) continue;
            auto output_mode = output_mode_ptr.data();

            auto output_mode_opt = output_mode->getWlrMode();
            if (!output_mode_opt || !output_mode_opt.has_value()) {
                qWarning() << "Output mode is not available, skipping.";
                continue;
            }

            if (output_mode_opt.value() == mode) {
                auto outputModeSizeOpt = output_mode->getSize();
                if (!outputModeSizeOpt.has_value()) return;
                if (!outputModeSizeOpt.value().isValid()) return;

                auto refreshOpt = output_mode->getRefresh();
                if (!refreshOpt.has_value()) return;

                auto outputModeSize = outputModeSizeOpt.value();
                auto refresh = refreshOpt.value();
                qDebug() << "Setting current mode to" << outputModeSize.width() << "x" << outputModeSize.height() << "@"
                         << refresh;
                m_current_mode = output_mode_ptr; // Set m_current_mode to same QSharedPointer as iterated output mode
                return;
            }
        }

//        auto meta_mode_ptr = addMode(mode); // Add the mode to the list of modes. If it already exists then we'll assign it to an existing Mode
//        if (meta_mode_ptr.isNull()) {
//            qWarning() << "Failed to add mode, meta_mode_ptr is null.";
//            return;
//        }
//        auto meta_mode = meta_mode_ptr.data();
//        if (meta_mode->isAvailable().value()) {
//            qDebug() << "(Mode already available) Current mode set to:" << meta_mode->getSize().value_or(QSize(0, 0))
//                     << "with refresh:" << meta_mode->getRefresh().value_or(0);
//            m_current_mode = meta_mode_ptr; // Set the current mode already since it is available
//        } else { // Not available yet
//            connect(meta_mode, &WaylandOutputMetaMode::done, this, [this, meta_mode_ptr, meta_mode]() {
//                // Set the current mode to the one that was just added
//                qDebug() << "(Mode done) Current mode set to:" << meta_mode->getSize().value_or(QSize(0, 0))
//                         << "with refresh:" << meta_mode->getRefresh().value_or(0);
//                m_current_mode = meta_mode_ptr;
//            });
//        }
    }

    void MetaHead::headDisconnected() {
        qDebug() << "Head disconnected for output: " << getIdentifier();
        m_head.clear();
        m_is_available = false;
        emit headNoLongerAvailable();
    }

    void MetaHead::setProperty(MetaHeadProperty::Property property, const QVariant &value) {
        bool changed = true;

        switch (property) {
            case MetaHeadProperty::Property::AdaptiveSync:
                m_adaptive_sync = static_cast<QtWayland::zwlr_output_head_v1::adaptive_sync_state>(value.toInt());
                qDebug() << "Setting adaptive sync on head" << getIdentifier() << "to" << m_adaptive_sync;
                break;
            case MetaHeadProperty::Property::Description:
                m_description = value.toString();
                qDebug() << "Output head finished, emitting headNoLongerAvailable: " << getIdentifier()
                         << " with description: " << m_description;
                break;
            case MetaHeadProperty::Property::Enabled:
                m_enabled = value.toBool();
                qInfo() << "Setting enabled state on head" << getIdentifier() << "to" << m_enabled;
                break;
            case MetaHeadProperty::Property::Make:
                m_make = value.toString();
                break;
            case MetaHeadProperty::Property::Model:
                m_model = value.toString();
                break;
            case MetaHeadProperty::Property::Name:
                m_name = value.toString();
                break;
            case MetaHeadProperty::Property::Position:
                m_position = value.toPoint();
                qDebug() << "Setting position on head" << getIdentifier() << "to" << m_position.x() << m_position.y();
                break;
            case MetaHeadProperty::Property::Scale:
                m_scale = value.toDouble();
                qDebug() << "Setting scale on head" << getIdentifier() << "to" << m_scale;
                break;
            case MetaHeadProperty::Property::SerialNumber:
                m_serial = value.toString();
                qDebug() << "Setting serial number on head" << getIdentifier() << "to" << m_serial;
                break;
            case MetaHeadProperty::Property::Transform:
                m_transform = value.toInt();
                qDebug() << "Setting transform on head" << getIdentifier() << "to" << m_transform;
                break;
            // None or invalid property
            case MetaHeadProperty::Property::None:
            default:
                qWarning() << "Unknown property" << property << "for output head";
                changed = false;
                break;
        }

        // If the property was not changed, do nothing
        if (!changed) return;

        emit propertyChanged(property, value);
    }

    // Anchoring/relative configuration accessors
    QString MetaHead::getRelativeOutput() {
        return m_relative_output;
    }

    bd::Outputs::Config::HorizontalAnchor::Type MetaHead::getHorizontalAnchor() const {
        return m_horizontal_anchor;
    }

    bd::Outputs::Config::VerticalAnchor::Type MetaHead::getVerticalAnchor() const {
        return m_vertical_anchor;
    }

    void MetaHead::setRelativeOutput(const QString &relative) {
        m_relative_output = relative;
        qDebug() << "Relative output set for head" << getIdentifier() << "relative:" << m_relative_output;
    }

    void MetaHead::setHorizontalAnchoring(bd::Outputs::Config::HorizontalAnchor::Type horizontal) {
        if (m_horizontal_anchor == horizontal) return;
        m_horizontal_anchor = horizontal;
        qDebug() << "Horizontal anchoring set for head" << getIdentifier()
                 << "h:" << bd::Outputs::Config::HorizontalAnchor::toString(m_horizontal_anchor);
    }


    void MetaHead::setVerticalAnchoring(bd::Outputs::Config::VerticalAnchor::Type vertical) {
        if (m_vertical_anchor == vertical) return;
        m_vertical_anchor = vertical;
        qDebug() << "Vertical anchoring set for head" << getIdentifier()
                 << "v:" << bd::Outputs::Config::VerticalAnchor::toString(m_vertical_anchor);
    }
}
