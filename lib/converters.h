/******************************************************************************
* Copyright (C) 2017 Kitsune Ral <kitsune-ral@users.sf.net>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include "util.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray> // Includes <QtCore/QJsonValue>
#include <QtCore/QJsonDocument>
#include <QtCore/QDate>
#include <QtCore/QUrlQuery>
#include <QtCore/QSet>
#include <QtCore/QVector>

#include <unordered_map>
#include <vector>
#if 0 // Waiting for C++17
#include <experimental/optional>

template <typename T>
using optional = std::experimental::optional<T>;
#endif

// Enable std::unordered_map<QString, T>
namespace std
{
    template <> struct hash<QString>
    {
        size_t operator()(const QString& s) const Q_DECL_NOEXCEPT
        {
            return qHash(s
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
                         , uint(qGlobalQHashSeed())
#endif
                         );
        }
    };
}

class QVariant;

namespace QMatrixClient
{
    // This catches anything implicitly convertible to QJsonValue/Object/Array
    inline auto toJson(const QJsonValue& val) { return val; }
    inline auto toJson(const QJsonObject& o) { return o; }
    inline auto toJson(const QJsonArray& arr) { return arr; }
    // Special-case QString to avoid ambiguity between QJsonValue
    // and QVariant (also, QString.isEmpty() is used in _impl::AddNode<> below)
    inline auto toJson(const QString& s) { return s; }

    inline QJsonArray toJson(const QStringList& strings)
    {
        return QJsonArray::fromStringList(strings);
    }

    inline QString toJson(const QByteArray& bytes)
    {
        return bytes.constData();
    }

    // QVariant is outrageously omnivorous - it consumes whatever is not
    // exactly matching the signature of other toJson overloads. The trick
    // below disables implicit conversion to QVariant through its numerous
    // non-explicit constructors.
    QJsonValue variantToJson(const QVariant& v);
    template <typename T>
    inline auto toJson(T&& /* const QVariant& or QVariant&& */ var)
        -> std::enable_if_t<std::is_same<std::decay_t<T>, QVariant>::value,
                            QJsonValue>
    {
        return variantToJson(var);
    }
    QJsonObject toJson(const QMap<QString, QVariant>& map);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    QJsonObject toJson(const QHash<QString, QVariant>& hMap);
#endif

    template <typename T>
    inline QJsonArray toJson(const std::vector<T>& vals)
    {
        QJsonArray ar;
        for (const auto& v: vals)
            ar.push_back(toJson(v));
        return ar;
    }

    template <typename T>
    inline QJsonArray toJson(const QVector<T>& vals)
    {
        QJsonArray ar;
        for (const auto& v: vals)
            ar.push_back(toJson(v));
        return ar;
    }

    template <typename T>
    inline QJsonObject toJson(const QSet<T>& set)
    {
        QJsonObject json;
        for (auto e: set)
            json.insert(toJson(e), QJsonObject{});
        return json;
    }

    template <typename T>
    inline QJsonObject toJson(const QHash<QString, T>& hashMap)
    {
        QJsonObject json;
        for (auto it = hashMap.begin(); it != hashMap.end(); ++it)
            json.insert(it.key(), toJson(it.value()));
        return json;
    }

    template <typename T>
    inline QJsonObject toJson(const std::unordered_map<QString, T>& hashMap)
    {
        QJsonObject json;
        for (auto it = hashMap.begin(); it != hashMap.end(); ++it)
            json.insert(it.key(), toJson(it.value()));
        return json;
    }

    template <typename T>
    struct FromJsonObject
    {
        T operator()(const QJsonObject& jo) const { return T(jo); }
    };

    template <typename T>
    struct FromJson
    {
        T operator()(const QJsonValue& jv) const
        {
            return FromJsonObject<T>()(jv.toObject());
        }
        T operator()(const QJsonDocument& jd) const
        {
            return FromJsonObject<T>()(jd.object());
        }
    };

    template <typename T>
    inline auto fromJson(const QJsonValue& jv)
    {
        return FromJson<T>()(jv);
    }

    template <typename T>
    inline auto fromJson(const QJsonDocument& jd)
    {
        return FromJson<T>()(jd);
    }

    template <> struct FromJson<bool>
    {
        auto operator()(const QJsonValue& jv) const { return jv.toBool(); }
    };

    template <> struct FromJson<int>
    {
        auto operator()(const QJsonValue& jv) const { return jv.toInt(); }
    };

    template <> struct FromJson<double>
    {
        auto operator()(const QJsonValue& jv) const { return jv.toDouble(); }
    };

    template <> struct FromJson<float>
    {
        auto operator()(const QJsonValue& jv) const { return float(jv.toDouble()); }
    };

    template <> struct FromJson<qint64>
    {
        auto operator()(const QJsonValue& jv) const { return qint64(jv.toDouble()); }
    };

    template <> struct FromJson<QString>
    {
        auto operator()(const QJsonValue& jv) const { return jv.toString(); }
    };

    template <> struct FromJson<QDateTime>
    {
        auto operator()(const QJsonValue& jv) const
        {
            return QDateTime::fromMSecsSinceEpoch(fromJson<qint64>(jv), Qt::UTC);
        }
    };

    template <> struct FromJson<QDate>
    {
        auto operator()(const QJsonValue& jv) const
        {
            return fromJson<QDateTime>(jv).date();
        }
    };

    template <> struct FromJson<QJsonArray>
    {
        auto operator()(const QJsonValue& jv) const
        {
            return jv.toArray();
        }
    };

    template <> struct FromJson<QByteArray>
    {
        auto operator()(const QJsonValue& jv) const
        {
            return fromJson<QString>(jv).toLatin1();
        }
    };

    template <> struct FromJson<QVariant>
    {
        QVariant operator()(const QJsonValue& jv) const;
    };

    template <typename VectorT>
    struct ArrayFromJson
    {
        auto operator()(const QJsonArray& ja) const
        {
            using size_type = typename VectorT::size_type;
            VectorT vect; vect.resize(size_type(ja.size()));
            std::transform(ja.begin(), ja.end(),
                           vect.begin(), FromJson<typename VectorT::value_type>());
            return vect;
        }
        auto operator()(const QJsonValue& jv) const
        {
            return operator()(jv.toArray());
        }
        auto operator()(const QJsonDocument& jd) const
        {
            return operator()(jd.array());
        }
    };

    template <typename T>
    struct FromJson<std::vector<T>> : ArrayFromJson<std::vector<T>>
    { };

    template <typename T>
    struct FromJson<QVector<T>> : ArrayFromJson<QVector<T>>
    { };

    template <typename T> struct FromJson<QList<T>>
    {
        auto operator()(const QJsonValue& jv) const
        {
            const auto jsonArray = jv.toArray();
            QList<T> sl; sl.reserve(jsonArray.size());
            std::transform(jsonArray.begin(), jsonArray.end(),
                           std::back_inserter(sl), FromJson<T>());
            return sl;
        }
    };

    template <> struct FromJson<QStringList> : FromJson<QList<QString>> { };

    template <> struct FromJson<QMap<QString, QVariant>>
    {
        QMap<QString, QVariant> operator()(const QJsonValue& jv) const;
    };

    template <typename T> struct FromJson<QSet<T>>
    {
        auto operator()(const QJsonValue& jv) const
        {
            const auto json = jv.toObject();
            QSet<T> s; s.reserve(json.size());
            for (auto it = json.begin(); it != json.end(); ++it)
                s.insert(it.key());
            return s;
        }
    };

    template <typename HashMapT>
    struct HashMapFromJson
    {
        auto operator()(const QJsonObject& jo) const
        {
            HashMapT h; h.reserve(jo.size());
            for (auto it = jo.begin(); it != jo.end(); ++it)
                h[it.key()] =
                    fromJson<typename HashMapT::mapped_type>(it.value());
            return h;
        }
        auto operator()(const QJsonValue& jv) const
        {
            return operator()(jv.toObject());
        }
        auto operator()(const QJsonDocument& jd) const
        {
            return operator()(jd.object());
        }
    };

    template <typename T>
    struct FromJson<std::unordered_map<QString, T>>
        : HashMapFromJson<std::unordered_map<QString, T>>
    { };

    template <typename T>
    struct FromJson<QHash<QString, T>> : HashMapFromJson<QHash<QString, T>>
    { };

#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    template <> struct FromJson<QHash<QString, QVariant>>
    {
        QHash<QString, QVariant> operator()(const QJsonValue& jv) const;
    };
#endif

    // Conditional insertion into a QJsonObject

    namespace _impl
    {
        template <typename ValT>
        inline void addTo(QJsonObject& o, const QString& k, ValT&& v)
        { o.insert(k, toJson(v)); }

        template <typename ValT>
        inline void addTo(QUrlQuery& q, const QString& k, ValT&& v)
        { q.addQueryItem(k, QStringLiteral("%1").arg(v)); }

        // OpenAPI is entirely JSON-based, which means representing bools as
        // textual true/false, rather than 1/0.
        inline void addTo(QUrlQuery& q, const QString& k, bool v)
        {
            q.addQueryItem(k, v ? QStringLiteral("true")
                                : QStringLiteral("false"));
        }

        inline void addTo(QUrlQuery& q, const QString& k, const QStringList& vals)
        {
            for (const auto& v: vals)
                q.addQueryItem(k, v);
        }

        inline void addTo(QUrlQuery& q, const QString&, const QJsonObject& vals)
        {
            for (auto it = vals.begin(); it != vals.end(); ++it)
                q.addQueryItem(it.key(), it.value().toString());
        }

        // This one is for types that don't have isEmpty()
        template <typename ValT, bool Force = true, typename = bool>
        struct AddNode
        {
            template <typename ContT, typename ForwardedT>
            static void impl(ContT& container, const QString& key,
                             ForwardedT&& value)
            {
                addTo(container, key, std::forward<ForwardedT>(value));
            }
        };

        // This one is for types that have isEmpty()
        template <typename ValT>
        struct AddNode<ValT, false,
                       decltype(std::declval<ValT>().isEmpty())>
        {
            template <typename ContT, typename ForwardedT>
            static void impl(ContT& container, const QString& key,
                             ForwardedT&& value)
            {
                if (!value.isEmpty())
                    AddNode<ValT>::impl(container,
                                        key, std::forward<ForwardedT>(value));
            }
        };

        // This is a special one that unfolds Omittable<>
        template <typename ValT, bool Force>
        struct AddNode<Omittable<ValT>, Force>
        {
            template <typename ContT, typename OmittableT>
            static void impl(ContT& container,
                             const QString& key, const OmittableT& value)
            {
                if (!value.omitted())
                    AddNode<ValT>::impl(container, key, value.value());
                else if (Force) // Edge case, no value but must put something
                    AddNode<ValT>::impl(container, key, QString{});
            }
        };

#if 0
        // This is a special one that unfolds optional<>
        template <typename ValT, bool Force>
        struct AddNode<optional<ValT>, Force>
        {
            template <typename ContT, typename OptionalT>
            static void impl(ContT& container,
                             const QString& key, const OptionalT& value)
            {
                if (value)
                    AddNode<ValT>::impl(container, key, value.value());
                else if (Force) // Edge case, no value but must put something
                    AddNode<ValT>::impl(container, key, QString{});
            }
        };
#endif

    }  // namespace _impl

    static constexpr bool IfNotEmpty = false;

    template <bool Force = true, typename ContT, typename ValT>
    inline void addParam(ContT& container, const QString& key, ValT&& value)
    {
        _impl::AddNode<std::decay_t<ValT>, Force>
                ::impl(container, key, std::forward<ValT>(value));
    }
}  // namespace QMatrixClient
