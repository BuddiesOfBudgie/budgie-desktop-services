#include <QDBusConnection>
#include <QPointer>

#include "metahead.hpp"
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

    bool MetaMode::available() const {
        return m_is_available.has_value() && m_is_available.value();
    }

    bool MetaMode::current() const {
        auto head = qobject_cast<bd::Outputs::Wlr::MetaHead*>(parent());
        if (!head) return false;
        auto currentMode = head->getCurrentMode();
        if (!currentMode) return false;
        // Compare pointer identity
        return currentMode.data() == this;
    }

    int MetaMode::height() const {
        auto size = m_size;
        if (size.isEmpty() || size.isNull()) return 0;
        return size.height();
    }

    QString MetaMode::id() const {
        return m_id;
    }

    bool MetaMode::preferred() const {
        return m_preferred.has_value() && m_preferred.value();
    }

    qulonglong MetaMode::refreshRate() const {
        return m_refresh;
    }

    int MetaMode::width() const {
        auto size = m_size;
        if (size.isEmpty() || size.isNull()) return 0;
        return size.width();
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

    // D-Bus registration
    void MetaMode::registerDbusService() {
        auto head = qobject_cast<bd::Outputs::Wlr::MetaHead*>(parent());
        if (!head) return;
        auto outputId = head->getIdentifier();
        QString objectPath = QString("/org/buddiesofbudgie/Services/Outputs/%1/Modes/%2").arg(outputId).arg(m_id);
        qDebug() << "Registering DBus service for mode" << m_id << "at path" << objectPath;
        if (!QDBusConnection::sessionBus().registerObject(objectPath, this, QDBusConnection::ExportAllContents)) {
            qWarning() << "Failed to register DBus object at path" << objectPath;
        }
    }

    // Setters

    void MetaMode::setMode(::zwlr_output_mode_v1 *wlr_mode) {
        if (wlr_mode == nullptr) {
            qWarning() << "Received null wlr_mode, doing nothing.";
            return;
        }
        unsetMode(); // Unset any existing mode

        qDebug() << "Creating new Mode with wlr mode:" << (void*)wlr_mode;
        auto mode = new Mode(wlr_mode);
        m_mode = QSharedPointer<Mode>(mode);

        connect(mode, &Mode::propertyChanged,
                this, &MetaMode::setProperty);
    }

    void MetaMode::unsetMode() {
        if (!m_mode.isNull()) {
            m_mode.clear();
        }
        m_is_available = std::make_optional<bool>(false);
    }

    bd::Outputs::OutputModeInfo MetaMode::toDBusStruct() const {
        bd::Outputs::OutputModeInfo info;
        info.id = m_id;
        info.width = m_size.width();
        info.height = m_size.height();
        info.refreshRate = m_refresh;
        info.preferred = m_preferred.has_value() && m_preferred.value();
        return info;
    }

    // Slots

    void MetaMode::modeDisconnected() {
        unsetMode();
        emit availabilityChanged(false);
        m_is_available = std::make_optional<bool>(false);
    }

    void MetaMode::setPreferred(bool preferred) {
        m_preferred = std::make_optional<bool>(preferred);
        emit preferredChanged(preferred);
    }

    void MetaMode::setProperty(MetaModeProperty::Property property, const QVariant &value) {
        bool changed = true;
        switch (property) {
            case MetaModeProperty::Property::Preferred:
                m_preferred = std::make_optional<bool>(value.toBool());
                emit preferredChanged(m_preferred.value());
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

        auto refresh = getRefresh();
        auto size = getSize();

        if (!refresh.has_value()) return;
        if (!size.has_value()) return;
        if (!size.value().isValid()) return;

        emit refreshRateChanged(refresh.value());
        emit sizeChanged(size.value());

        // Compute ID string as {width}_{height}_{refresh} and ensure it's DBus object-path safe
        const auto sz = size.value();
        const auto ref = refresh.value();
        m_id = QString("%1_%2_%3").arg(sz.width()).arg(sz.height()).arg(ref);

        m_is_available = std::make_optional<bool>(true);
        emit availabilityChanged(true);
        emit done();
    }
}
