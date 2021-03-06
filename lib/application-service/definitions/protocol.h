/******************************************************************************
 * THIS FILE IS GENERATED - ANY EDITS WILL BE OVERWRITTEN
 */

#pragma once

#include "converters.h"

#include <QtCore/QHash>
#include <QtCore/QJsonObject>
#include "converters.h"
#include <QtCore/QVector>

namespace QMatrixClient
{
    // Data structures

    /// Definition of valid values for a field.
    struct FieldType
    {
        /// A regular expression for validation of a field's value. This may be relatively
        /// coarse to verify the value as the application service providing this protocol
        /// may apply additional validation or filtering.
        QString regexp;
        /// An placeholder serving as a valid example of the field value.
        QString placeholder;
    };

    QJsonObject toJson(const FieldType& pod);

    template <> struct FromJsonObject<FieldType>
    {
        FieldType operator()(const QJsonObject& jo) const;
    };

    struct ProtocolInstance
    {
        /// A human-readable description for the protocol, such as the name.
        QString desc;
        /// An optional content URI representing the protocol. Overrides the one provided
        /// at the higher level Protocol object.
        QString icon;
        /// Preset values for ``fields`` the client may use to search by.
        QJsonObject fields;
        /// A unique identifier across all instances.
        QString networkId;
    };

    QJsonObject toJson(const ProtocolInstance& pod);

    template <> struct FromJsonObject<ProtocolInstance>
    {
        ProtocolInstance operator()(const QJsonObject& jo) const;
    };

    struct ThirdPartyProtocol
    {
        /// Fields which may be used to identify a third party user. These should be
        /// ordered to suggest the way that entities may be grouped, where higher
        /// groupings are ordered first. For example, the name of a network should be
        /// searched before the nickname of a user.
        QStringList userFields;
        /// Fields which may be used to identify a third party location. These should be
        /// ordered to suggest the way that entities may be grouped, where higher
        /// groupings are ordered first. For example, the name of a network should be
        /// searched before the name of a channel.
        QStringList locationFields;
        /// A content URI representing an icon for the third party protocol.
        QString icon;
        /// The type definitions for the fields defined in the ``user_fields`` and 
        /// ``location_fields``. Each entry in those arrays MUST have an entry here. The
        /// ``string`` key for this object is field name itself.
        /// 
        /// May be an empty object if no fields are defined.
        QHash<QString, FieldType> fieldTypes;
        /// A list of objects representing independent instances of configuration.
        /// For example, multiple networks on IRC if multiple are provided by the
        /// same application service.
        QVector<ProtocolInstance> instances;
    };

    QJsonObject toJson(const ThirdPartyProtocol& pod);

    template <> struct FromJsonObject<ThirdPartyProtocol>
    {
        ThirdPartyProtocol operator()(const QJsonObject& jo) const;
    };

} // namespace QMatrixClient
