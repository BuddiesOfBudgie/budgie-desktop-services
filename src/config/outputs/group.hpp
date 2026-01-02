#pragma once

#include <qtmetamacros.h>
#include <QObject>
#include <QSharedPointer>

#include "outputs/wlr/metahead.hpp"
#include "output.hpp"

namespace bd::Config::Outputs {
    class Group : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(bool preferred READ preferred WRITE setPreferred NOTIFY preferredChanged)
        // Stored identifiers
        Q_PROPERTY(QString storedPrimaryOutputIdentifier READ storedPrimaryOutputIdentifier WRITE setStoredPrimaryOutputIdentifier NOTIFY storedPrimaryOutputIdentifierChanged)
        Q_PROPERTY(QList<QString> storedIdentifiers READ storedIdentifiers WRITE setStoredIdentifiers NOTIFY storedIdentifiersChanged)
        // Internal config representations for stored identifiers
        Q_PROPERTY(QList<QSharedPointer<Output>> outputConfigs READ outputConfigs WRITE setOutputConfigs NOTIFY outputConfigsChanged)

    Q_SIGNALS:
        void nameChanged(const QString& name);
        void storedPrimaryOutputIdentifierChanged(const QString& storedPrimaryOutputIdentifier);
        void storedIdentifiersChanged(const QStringList& storedIdentifiers);
        void outputConfigsChanged(const QList<QSharedPointer<Output>>& outputConfigs);
        void preferredChanged(bool preferred);

    public:
        Group(QObject* parent = nullptr);
        Group(const toml::value& v, QObject* parent = nullptr);
        ~Group() = default;

        // Property getters
        QString name() const;
        QString storedPrimaryOutputIdentifier() const;
        QStringList storedIdentifiers() const;
        bool preferred() const;
        QList<QSharedPointer<Output>> outputConfigs() const;

        // Property setters
        void setName(const QString& name);
        void setPreferred(bool preferred);
        void setStoredPrimaryOutputIdentifier(const QString& storedPrimaryOutputIdentifier);
        void setStoredIdentifiers(QStringList storedIdentifiers);
        void setOutputConfigs(const QList<QSharedPointer<Output>>& outputConfigs);

        // Other methods
        void addMetaHead(QSharedPointer<bd::Outputs::Wlr::MetaHead> metaHead);
        void removeMetaHead(QSharedPointer<bd::Outputs::Wlr::MetaHead> metaHead);
        void setPrimaryMetaHead(QSharedPointer<bd::Outputs::Wlr::MetaHead> metaHead);

        void apply();
        toml::ordered_value toToml();
    
    private:
        QSharedPointer<Output> getOutputForIdentifier(const QString& identifier);
        QString m_name;
        bool m_preferred;
        QString m_stored_primary_output_identifier;
        QStringList m_stored_identifiers;
        QList<QSharedPointer<Output>> m_output_configs;
    };
}