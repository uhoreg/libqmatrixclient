/******************************************************************************
 * THIS FILE IS GENERATED - ANY EDITS WILL BE OVERWRITTEN
 */

#include "event_context.h"

#include "converters.h"

#include <QtCore/QStringBuilder>

using namespace QMatrixClient;

static const auto basePath = QStringLiteral("/_matrix/client/r0");

class GetEventContextJob::Private
{
    public:
        QString begin;
        QString end;
        RoomEvents eventsBefore;
        RoomEventPtr event;
        RoomEvents eventsAfter;
        StateEvents state;
};

BaseJob::Query queryToGetEventContext(Omittable<int> limit)
{
    BaseJob::Query _q;
    addParam<IfNotEmpty>(_q, QStringLiteral("limit"), limit);
    return _q;
}

QUrl GetEventContextJob::makeRequestUrl(QUrl baseUrl, const QString& roomId, const QString& eventId, Omittable<int> limit)
{
    return BaseJob::makeRequestUrl(std::move(baseUrl),
            basePath % "/rooms/" % roomId % "/context/" % eventId,
            queryToGetEventContext(limit));
}

static const auto GetEventContextJobName = QStringLiteral("GetEventContextJob");

GetEventContextJob::GetEventContextJob(const QString& roomId, const QString& eventId, Omittable<int> limit)
    : BaseJob(HttpVerb::Get, GetEventContextJobName,
        basePath % "/rooms/" % roomId % "/context/" % eventId,
        queryToGetEventContext(limit))
    , d(new Private)
{
}

GetEventContextJob::~GetEventContextJob() = default;

const QString& GetEventContextJob::begin() const
{
    return d->begin;
}

const QString& GetEventContextJob::end() const
{
    return d->end;
}

RoomEvents&& GetEventContextJob::eventsBefore()
{
    return std::move(d->eventsBefore);
}

RoomEventPtr&& GetEventContextJob::event()
{
    return std::move(d->event);
}

RoomEvents&& GetEventContextJob::eventsAfter()
{
    return std::move(d->eventsAfter);
}

StateEvents&& GetEventContextJob::state()
{
    return std::move(d->state);
}

BaseJob::Status GetEventContextJob::parseJson(const QJsonDocument& data)
{
    auto json = data.object();
    d->begin = fromJson<QString>(json.value("start"_ls));
    d->end = fromJson<QString>(json.value("end"_ls));
    d->eventsBefore = fromJson<RoomEvents>(json.value("events_before"_ls));
    d->event = fromJson<RoomEventPtr>(json.value("event"_ls));
    d->eventsAfter = fromJson<RoomEvents>(json.value("events_after"_ls));
    d->state = fromJson<StateEvents>(json.value("state"_ls));
    return Success;
}

