#include "outputs/state.hpp"
#include "sys/SysInfo.hpp"
#include "output.hpp"

namespace bd::Config::Outputs {
    Output::Output(QObject* parent) : QObject(parent),
    m_identifier(""),
    m_width(0),
    m_height(0),
    m_refresh(0),
    m_x(0),
    m_y(0),
    m_relativeOutput(""),
    m_horizontalAnchor(bd::Outputs::Config::HorizontalAnchor::Type::None),
    m_verticalAnchor(bd::Outputs::Config::VerticalAnchor::Type::None),
    m_transform(0),
    m_adaptiveSync(0),
    m_primary(false),
    m_scale(1.0),
    m_disabled(false) {}

    Output::Output(const toml::value& v, QObject* parent) : QObject(parent) {
        m_identifier = QString::fromStdString(toml::find<std::string>(v, "identifier"));
        m_width = toml::find<int>(v, "width");
        m_height = toml::find<int>(v, "height");
        m_refresh = toml::find<qulonglong>(v, "refresh");
        m_x = toml::find_or<int>(v, "x", 0);
        m_y = toml::find_or<int>(v, "y", 0);
        m_relativeOutput = QString::fromStdString(toml::find_or<std::string>(v, "relative_output", ""));
        m_horizontalAnchor = bd::Outputs::Config::HorizontalAnchor::fromString(toml::find_or<std::string>(v, "horizontal_anchor", "none"));
        m_verticalAnchor = bd::Outputs::Config::VerticalAnchor::fromString(toml::find_or<std::string>(v, "vertical_anchor", "none"));
        m_transform = toml::find_or<quint16>(v, "rotation", 0);
        m_adaptiveSync = toml::find_or<uint32_t>(v, "adaptive_sync", 0);
        m_primary = toml::find_or<bool>(v, "primary", false);
        m_disabled = toml::find_or<bool>(v, "disabled", false);
        m_scale = toml::find_or<qreal>(v, "scale", 1.0);
    }

    // Property getters

    QSharedPointer<bd::Outputs::Wlr::MetaHead> Output::getMetaHead() {
        // TODO(JoshStrobl): Consider tweaking this in future with reference setting the head instead of getting it from the manager.
        auto man = bd::Outputs::State::instance().getManager();
        if (!man) return nullptr;
        return man->getOutputHead(this->m_identifier);
    }

    QString Output::identifier() const {
        return this->m_identifier;
    }

    int Output::width() {
        return this->m_width;
    }
    
    int Output::height() {
        return this->m_height;
    }

    qulonglong Output::refresh() {
        return this->m_refresh;
    }
    
    int Output::x() {
        return this->m_x;
    }

    int Output::y() {
        return this->m_y;
    }
    
    QString Output::relativeOutput() {
        return this->m_relativeOutput;
    }

    bd::Outputs::Config::HorizontalAnchor::Type Output::horizontalAnchor() {
        return this->m_horizontalAnchor;
    }
    
    bd::Outputs::Config::VerticalAnchor::Type Output::verticalAnchor() {
        return this->m_verticalAnchor;
    }

    quint16 Output::transform() {
        return this->m_transform;
    }
    
    
    uint32_t Output::adaptiveSync() {
        return this->m_adaptiveSync;
    }

    bool Output::primary() {
        return this->m_primary;
    }

    bool Output::disabled() {
        return this->m_disabled;
    }

    qreal Output::scale() {
        return this->m_scale;
    }

    // Property setters

    void Output::setIdentifier(const QString& identifier) {
        this->m_identifier = identifier;
        emit identifierChanged(identifier);
    }
    
    void Output::setWidth(int width) {
        this->m_width = width;
        emit widthChanged(width);
    }

    void Output::setHeight(int height) {
        this->m_height = height;
        emit heightChanged(height);
    }
    
    void Output::setRefresh(qulonglong refresh) {
        this->m_refresh = refresh;
        emit refreshChanged(refresh);
    }

    void Output::setX(int x) {
        this->m_x = x;
        emit xChanged(x);
    }

    void Output::setY(int y) {
        this->m_y = y;
        emit yChanged(y);
    }

    void Output::setRelativeOutput(const QString& relativeOutput) {
        this->m_relativeOutput = relativeOutput;
        emit relativeOutputChanged(relativeOutput);
    }

    void Output::setHorizontalAnchor(bd::Outputs::Config::HorizontalAnchor::Type horizontalAnchor) {
        this->m_horizontalAnchor = horizontalAnchor;
        emit horizontalAnchorChanged(horizontalAnchor);
    }

    void Output::setVerticalAnchor(bd::Outputs::Config::VerticalAnchor::Type verticalAnchor) {
        this->m_verticalAnchor = verticalAnchor;
        emit verticalAnchorChanged(verticalAnchor);
    }

    void Output::setTransform(quint16 transform) {
        this->m_transform = transform;
        emit transformChanged(transform);
    }

    void Output::setAdaptiveSync(uint32_t adaptiveSync) {
        this->m_adaptiveSync = adaptiveSync;
        emit adaptiveSyncChanged(adaptiveSync);
    }

    void Output::setScale(qreal scale) {
        this->m_scale = scale;
        emit scaleChanged(scale);
    }

    void Output::setPrimary(bool primary) {
        this->m_primary = primary;
        emit primaryChanged(primary);
    }

    void Output::setDisabled(bool disabled) {
        this->m_disabled = disabled;
        emit disabledChanged(disabled);
    }

    // Other methods

    toml::ordered_value Output::toToml() {
        toml::ordered_value config_table(toml::ordered_table {});
        config_table.as_table_fmt().fmt = toml::table_format::multiline;

        config_table["identifier"] = this->identifier().toStdString();
        config_table["width"] = this->width();
        config_table["height"] = this->height();
        config_table["refresh"] = static_cast<qulonglong>(this->refresh());

        if (SysInfo::instance().isShimMode()) {
            config_table["x"] = this->x();
            config_table["y"] = this->y();
        } else {
            config_table["relative_output"] = this->relativeOutput().toStdString();
            config_table["horizontal_anchor"] = bd::Outputs::Config::HorizontalAnchor::toStringStd(this->horizontalAnchor());
            config_table["vertical_anchor"] = bd::Outputs::Config::VerticalAnchor::toStringStd(this->verticalAnchor());
        }

        config_table["scale"] = this->scale();
        config_table["rotation"] = this->transform();
        config_table["adaptive_sync"] = this->adaptiveSync();
        config_table["primary"] = this->primary();
        config_table["disabled"] = this->disabled();

        return config_table;
    }

    void Output::updateFromHead() {
        auto head = getMetaHead();
        if (!head) return;

        this->setWidth(head->width());
        this->setHeight(head->height());
        this->setRefresh(head->refreshRate());
        this->setX(head->x());
        this->setY(head->y());
        this->setRelativeOutput(head->relativeTo());
        this->setHorizontalAnchor(head->getHorizontalAnchor());
        this->setVerticalAnchor(head->getVerticalAnchor());
        this->setTransform(head->transform());
        this->setAdaptiveSync(head->adaptiveSync());
        this->setScale(head->scale());
        this->setPrimary(head->primary());
        this->setDisabled(!head->enabled());
    }
}