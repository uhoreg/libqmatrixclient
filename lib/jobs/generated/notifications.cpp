/******************************************************************************
 * THIS FILE IS GENERATED - ANY EDITS WILL BE OVERWRITTEN
 */

#include "notifications.h"

#include <QtCore/QStringBuilder>

using namespace QMatrixClient;

static const auto basePath = QStringLiteral("/_matrix/client/r0");

namespace QMatrixClient
{
    template <> struct FromJson<GetNotificationsJob::Notification>
    {
        GetNotificationsJob::Notification operator()(const QJsonValue& jv)
        {
            const auto& o = jv.toObject();
            GetNotificationsJob::Notification result;
            result.actions =
                fromJson<QVector<QJsonObject>>(o.value("actions"));
            result.event =
                fromJson<EventPtr>(o.value("event"));
            result.profileTag =
                fromJson<QString>(o.value("profile_tag"));
            result.read =
                fromJson<bool>(o.value("read"));
            result.roomId =
                fromJson<QString>(o.value("room_id"));
            result.ts =
                fromJson<qint64>(o.value("ts"));
            
            return result;
        }
    };
} // namespace QMatrixClient

class GetNotificationsJob::Private
{
    public:
        QString nextToken;
        std::vector<Notification> notifications;
};

BaseJob::Query queryToGetNotifications(const QString& from, int limit, const QString& only)
{
    BaseJob::Query _q;
    if (!from.isEmpty())
        _q.addQueryItem("from", from);
    _q.addQueryItem("limit", QString("%1").arg(limit));
    if (!only.isEmpty())
        _q.addQueryItem("only", only);
    return _q;
}

QUrl GetNotificationsJob::makeRequestUrl(QUrl baseUrl, const QString& from, int limit, const QString& only)
{
    return BaseJob::makeRequestUrl(std::move(baseUrl),
            basePath % "/notifications",
            queryToGetNotifications(from, limit, only));
}

GetNotificationsJob::GetNotificationsJob(const QString& from, int limit, const QString& only)
    : BaseJob(HttpVerb::Get, "GetNotificationsJob",
        basePath % "/notifications",
        queryToGetNotifications(from, limit, only))
    , d(new Private)
{
}

GetNotificationsJob::~GetNotificationsJob() = default;

const QString& GetNotificationsJob::nextToken() const
{
    return d->nextToken;
}

const std::vector<GetNotificationsJob::Notification>& GetNotificationsJob::notifications() const
{
    return d->notifications;
}

BaseJob::Status GetNotificationsJob::parseJson(const QJsonDocument& data)
{
    auto json = data.object();
    d->nextToken = fromJson<QString>(json.value("next_token"));
    if (!json.contains("notifications"))
        return { JsonParseError,
            "The key 'notifications' not found in the response" };
    d->notifications = fromJson<std::vector<Notification>>(json.value("notifications"));
    return Success;
}

