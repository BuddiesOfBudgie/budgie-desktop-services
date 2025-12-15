#include <QPointer>
#include "metamode.hpp"

namespace bd::Outputs::Wlr {
    MetaMode::MetaMode(QObject *parent, ::zwlr_output_mode_v1 *wlr_mode)
            : QObject(parent), m_id(QString()), m_mode(QSharedPointer<Mode>(nullptr)), m_refresh(0), m_preferred(std::nullopt), m_size(QSize{0, 0}),
              m_is_available(std::nullopt) {
        setMode(wlr_mode);
    }

    MetaMode::~MetaMode() {
        unsetMode(); // Unset the mode to ensure no references are held
        m_preferred = std::nullopt; // Clear preferred state
    }

    QString MetaMode::getId() {
        return m_id;
    }

    std::optional<qulonglong> MetaMode::getRefresh() {
        if (m_refresh == 0) return std::nullopt;
        return std::make_optional(m_refresh);
    }

    std::optional<QSize> MetaMode::getSize() {
        if (m_size.isEmpty() || m_size.isNull()) return std::nullopt;
        return std::make_optional(m_size);
    }

    std::optional<const ::zwlr_output_mode_v1*> MetaMode::getWlrMode() {
        if (!m_mode || m_mode.isNull()) return std::nullopt;
        return m_mode->getWlrMode();
    }

    std::optional<bool> MetaMode::isAvailable() {
        return m_is_available;
    }

    std::optional<bool> MetaMode::isPreferred() {
        return m_preferred;
    }

    bool MetaMode::isSameAs(MetaMode *mode) {
        if (mode == nullptr) { return false; }

        auto r_refresh = mode->getRefresh();
        if (!r_refresh) { return false; }

        auto r_size = mode->getSize();
        if (!r_size) { return false; }

        auto refresh = getRefresh();
        if (!refresh) { return false; }

        auto size = getSize();
        if (!size) { return false; }

        auto same = r_refresh.value() == refresh.value() && r_size.value() == size.value();
        if (same) {
            qDebug() << "Mode is same as ours, with refresh:" << r_refresh.value() << "and size:" << r_size.value();
        }

        return same;
    }

    // Setters

    void MetaMode::setMode(::zwlr_output_mode_v1 *wlr_mode) {
        if (wlr_mode == nullptr) {
            qWarning() << "Received null wlr_mode, doing nothing.";
            return;
        }
        unsetMode(); // Unset any existing mode

        qDebug() << "Setting new mode with Wayland object:" << (void*)wlr_mode;
        auto mode = new Mode(wlr_mode);
        m_mode = QSharedPointer<Mode>(mode);
//        connect(mode, &Mode::modeFinished, this, &MetaMode::modeDisconnected);

        connect(mode, &Mode::propertyChanged,
                this, &MetaMode::setProperty);
    }

    void MetaMode::unsetMode() {
        if (!m_mode.isNull()) {
            m_mode.clear();
        }
        m_is_available = std::make_optional<bool>(false);
    }

    // Slots

    void MetaMode::modeDisconnected() {
        unsetMode();
        emit modeNoLongerAvailable();
        m_is_available = std::make_optional<bool>(false);
    }

    void MetaMode::setPreferred(bool preferred) {
        m_preferred = std::make_optional<bool>(preferred);
        emit propertyChanged(MetaModeProperty::Property::Preferred, QVariant::fromValue(preferred));
    }

    void MetaMode::setProperty(MetaModeProperty::Property property, const QVariant &value) {
        bool changed = true;
        switch (property) {
            case MetaModeProperty::Property::Preferred:
                m_preferred = std::make_optional<bool>(value.toBool());
                break;
            case MetaModeProperty::Property::Refresh:
                m_refresh = static_cast<qulonglong>(value.toULongLong());
                break;
            case MetaModeProperty::Property::Size:
                m_size = value.toSize();
                break;
            // None or invalid property
            case MetaModeProperty::Property::None:
            default:
                changed = false;
                break;
        }

        // If the property was not changed, do nothing
        if (!changed) return;

        emit propertyChanged(property, value);

        auto refresh = getRefresh();
        auto size = getSize();

        if (!refresh.has_value()) return;
        if (!size.has_value()) return;
        if (!size.value().isValid()) return;

        // Compute ID string as {width}_{height}_{refresh} and ensure it's DBus object-path safe
        const auto sz = size.value();
        const auto ref = refresh.value();
        m_id = QString("%1_%2_%3").arg(sz.width()).arg(sz.height()).arg(ref);
        // no decimal in integer refresh

        m_is_available = true;
        emit done();
    }
}
