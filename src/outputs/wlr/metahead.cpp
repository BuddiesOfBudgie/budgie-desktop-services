#include <QCryptographicHash>
#include <QtAlgorithms>
#include <optional>
#include <QDBusConnection>
#include <QSize>

#include "metahead.hpp"
#include "head.hpp"
#include "config/outputs/state.hpp"
#include "outputs/config/enums/anchors.hpp"
#include "sys/SysInfo.hpp"

namespace bd::Outputs::Wlr {
    MetaHead::MetaHead(QObject *parent)
            : QObject(parent),
              m_built_in(true),
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


    QPoint MetaHead::getPosition() {
        return m_position;
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

    bool MetaHead::builtIn() {
        // Generate identifier if necessary
        getIdentifier();
        // Return if identifier exists
        return m_serial.isNull() || m_serial.isEmpty();
    }

    bd::Outputs::OutputModeInfo MetaHead::currentMode() const {
        if (!m_current_mode) {
            bd::Outputs::OutputModeInfo empty;
            empty.id = QString();
            empty.width = 0;
            empty.height = 0;
            empty.refreshRate = 0;
            empty.preferred = false;
            return empty;
        }
        return m_current_mode->toDBusStruct();
    }

    bd::Outputs::OutputModesMap MetaHead::modes() const {
        bd::Outputs::OutputModesMap modes;
        for (const auto& mode_ptr: m_output_modes) {
            if (!mode_ptr) continue;
            modes.insert(mode_ptr->id(), mode_ptr->toDBusStruct());
        }
        return modes;
    }

    QString MetaHead::serial() const {
        return const_cast<MetaHead*>(this)->getIdentifier();
    }

    QString MetaHead::name() const {
        return m_name;
    }

    QString MetaHead::description() const {
        return m_description;
    }

    QString MetaHead::make() const {
        return m_make;
    }

    QString MetaHead::model() const {
        return m_model;
    }

    bool MetaHead::enabled() const {
        return m_enabled;
    }

    int MetaHead::width() const {
        auto mode = m_current_mode;
        if (mode) return mode->getSize().value_or(QSize(0, 0)).width();
        return 0;
    }

    int MetaHead::height() const {
        auto mode = m_current_mode;
        if (mode) return mode->getSize().value_or(QSize(0, 0)).height();
        return 0;
    }

    int MetaHead::x() const {
        return m_position.x();
    }

    int MetaHead::y() const {
        return m_position.y();
    }

    double MetaHead::scale() const {
        return m_scale;
    }

    qulonglong MetaHead::refreshRate() const {
        auto mode = m_current_mode;
        if (mode) return static_cast<qulonglong>(mode->getRefresh().value_or(0.0));
        return 0;
    }

    quint16 MetaHead::transform() const {
        return static_cast<quint16>(m_transform);
    }

    uint MetaHead::adaptiveSync() const {
        return static_cast<uint>(m_adaptive_sync);
    }

    bool MetaHead::primary() const {
        return m_primary;
    }

    QString MetaHead::mirrorOf() const {
        return QString(); /* TODO: implement if available */
    }

    QString MetaHead::horizontalAnchor() const {
        return bd::Outputs::Config::HorizontalAnchor::toString(m_horizontal_anchor);
    }

    QString MetaHead::verticalAnchor() const {
        return bd::Outputs::Config::VerticalAnchor::toString(m_vertical_anchor);
    }

    QString MetaHead::relativeTo() const {
        return m_relative_output;
    }

    // Setters

    void MetaHead::setHead(::zwlr_output_head_v1 *wlr_head) {
        auto head = new bd::Outputs::Wlr::Head(this, wlr_head);
        m_head = QSharedPointer<bd::Outputs::Wlr::Head>(head);
        m_is_available = true;
        emit headAvailable();
        connect(head, &bd::Outputs::Wlr::Head::headFinished, this, &MetaHead::headDisconnected);
        connect(head, &bd::Outputs::Wlr::Head::modeAdded, this, &MetaHead::addMode);
        connect(head, &bd::Outputs::Wlr::Head::modeChanged, this, &MetaHead::currentZwlrModeChanged);
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
        emit positionChanged(m_position);
        emit stateChanged();
    }

    void MetaHead::setPrimary(bool primary) {
        if (m_primary == primary) return;
        m_primary = primary;
        emit primaryChanged(m_primary);
        emit stateChanged();
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
            qDebug() << "Done triggered for mode" << output_mode->id() << "on head" << getIdentifier();
            auto found_matching_mode = false;
            auto matching_mode_is_current = false;

            qDebug() << "Checking existing modes for any matches to this one.";
            for (const auto &mode_ptr: m_output_modes) {
                if (!mode_ptr) continue;
                auto existing_mode = mode_ptr.data();
                qDebug() << "Checking existing mode" << existing_mode->id() << "for a match.";
                // Already exists, delete the existing mode and add the new one
                if (existing_mode->isSameAs(output_mode)) {
                    qDebug() << "Found an output mode (ID: " << existing_mode->id() << ") that matches one we already have, deleting the old one.";
                    found_matching_mode = true;
                    m_output_modes.removeOne(mode_ptr);

                    matching_mode_is_current = (m_current_mode == mode_ptr);
                    
                    break;
                }
            }

            // Doesn't already exist, add it
            qDebug() << "Adding new output mode (ID: " << output_mode->id() << ") to head: " << getIdentifier() << " with size: "
                     << output_mode->getSize().value_or(QSize(0, 0))
                     << " and refresh: " << static_cast<qulonglong>(output_mode->getRefresh().value_or(0));

            m_output_modes.append(shared_ptr);

            emit modesChanged();

            if (found_matching_mode && matching_mode_is_current) {
                qDebug() << "The old matching mode was the current mode, setting the current mode to the new one.";
                m_current_mode = shared_ptr;
                emit currentModeChanged(currentMode());
            }

            emit stateChanged();
        });

        return shared_ptr;
    }

    void MetaHead::currentZwlrModeChanged(::zwlr_output_mode_v1 *mode) {
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

                emit modesChanged();
                emit widthChanged(outputModeSize.width());
                emit heightChanged(outputModeSize.height());
                emit refreshRateChanged(refresh);
                emit currentModeChanged(currentMode());
                emit stateChanged();
                return;
            }
        }
    }

    void MetaHead::headDisconnected() {
        qDebug() << "Head disconnected for output: " << getIdentifier();
        m_head.clear();
        m_is_available = false;
        emit headNoLongerAvailable();
        emit stateChanged();
    }

    void MetaHead::setProperty(MetaHeadProperty::Property property, const QVariant &value) {
        bool changed = true;

        switch (property) {
            case MetaHeadProperty::Property::AdaptiveSync:
                m_adaptive_sync = static_cast<QtWayland::zwlr_output_head_v1::adaptive_sync_state>(value.toInt());
                qDebug() << "Setting adaptive sync on head" << getIdentifier() << "to" << m_adaptive_sync;
                emit adaptiveSyncChanged(m_adaptive_sync);
                emit stateChanged();
                break;
            case MetaHeadProperty::Property::Description:
                m_description = value.toString();
                qDebug() << "Setting description on head" << getIdentifier() << "to" << m_description;
                emit descriptionChanged(m_description);
                break;
            case MetaHeadProperty::Property::Enabled:
                m_enabled = value.toBool();
                qInfo() << "Setting enabled state on head" << getIdentifier() << "to" << m_enabled;
                emit enabledChanged(m_enabled);
                emit stateChanged();
                break;
            case MetaHeadProperty::Property::Make:
                m_make = value.toString();
                qDebug() << "Setting make on head" << getIdentifier() << "to" << m_make;
                emit makeChanged(m_make);
                break;
            case MetaHeadProperty::Property::Model:
                m_model = value.toString();
                qDebug() << "Setting model on head" << getIdentifier() << "to" << m_model;
                emit modelChanged(m_model);
                break;
            case MetaHeadProperty::Property::Name:
                m_name = value.toString();
                qDebug() << "Setting name on head" << getIdentifier() << "to" << m_name;
                emit nameChanged(m_name);
                break;
            case MetaHeadProperty::Property::Position:
                m_position = value.toPoint();
                qDebug() << "Setting position on head" << getIdentifier() << "to" << m_position.x() << m_position.y();
                emit positionChanged(m_position);
                emit stateChanged();
                break;
            case MetaHeadProperty::Property::Scale:
                m_scale = value.toDouble();
                qDebug() << "Setting scale on head" << getIdentifier() << "to" << m_scale;
                emit scaleChanged(m_scale);
                emit stateChanged();
                break;
            case MetaHeadProperty::Property::SerialNumber:
                m_serial = value.toString();
                qDebug() << "Setting serial number on head" << getIdentifier() << "to" << m_serial;
                emit serialChanged(m_serial);
                break;
            case MetaHeadProperty::Property::Transform:
                m_transform = value.toInt();
                qDebug() << "Setting transform on head" << getIdentifier() << "to" << m_transform;
                emit transformChanged(m_transform);
                emit stateChanged();
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

        // If we are in shim mode, immediately save the state whenever the head changes
        if (SysInfo::instance().isShimMode()) {
            bd::Config::Outputs::State::instance().save();
        }
    }

    // Anchoring/relative configuration accessors

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

    // D-Bus registration
    void MetaHead::registerDbusService() {
        QString objectPath = QString("/org/buddiesofbudgie/Services/Outputs/%1").arg(getIdentifier());
        qInfo() << "Registering DBus service for output" << getIdentifier() << "at path" << objectPath;
        if (!QDBusConnection::sessionBus().registerObject(objectPath, this, QDBusConnection::ExportAllContents)) {
            qCritical() << "Failed to register DBus object at path" << objectPath;
            return;
        }

        // Register all modes for this output
        for (const auto& mode : m_output_modes) {
            if (!mode) continue;
            mode->registerDbusService();
        }
    }
}
