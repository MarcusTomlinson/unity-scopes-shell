/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Michal Hruby <michal.hruby@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// self
#include "resultsmodel.h"

// local
#include "utils.h"
#include "iconutils.h"

#include <QDebug>

namespace scopes_ng {

using namespace unity;

ResultsModel::ResultsModel(QObject* parent)
 : unity::shell::scopes::ResultsModelInterface(parent)
 , m_maxAttributes(2)
{
}

QString ResultsModel::categoryId() const
{
    return m_categoryId;
}

void ResultsModel::setCategoryId(QString const& id)
{
    if (m_categoryId != id) {
        m_categoryId = id;
        Q_EMIT categoryIdChanged();
    }
}

void ResultsModel::setComponentsMapping(QHash<QString, QString> const& mapping)
{
    std::unordered_map<std::string, std::string> newMapping;
    for (auto it = mapping.begin(); it != mapping.end(); ++it) {
        newMapping[it.key().toStdString()] = it.value().toStdString();
    }

    if (rowCount() > 0) {
        beginResetModel();
        m_componentMapping.swap(newMapping);
        endResetModel();
    } else {
        m_componentMapping.swap(newMapping);
    }
}

void ResultsModel::setMaxAtrributesCount(int count)
{
    m_maxAttributes = count;
}

void ResultsModel::addResults(QList<std::shared_ptr<unity::scopes::CategorisedResult>> const& results)
{
    if (results.count() == 0) return;
    
    beginInsertRows(QModelIndex(), m_results.count(), m_results.count() + results.count() - 1);
    Q_FOREACH(std::shared_ptr<scopes::CategorisedResult> const& result, results) {
        m_results.append(result);
    }
    endInsertRows();

    Q_EMIT countChanged();
}

void ResultsModel::clearResults()
{
    if (m_results.count() == 0) return;

    beginRemoveRows(QModelIndex(), 0, m_results.count() - 1);
    m_results.clear();
    endRemoveRows();

    Q_EMIT countChanged();
}

int ResultsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return m_results.count();
}

int ResultsModel::count() const
{
    return m_results.count();
}

QVariant
ResultsModel::componentValue(scopes::CategorisedResult const* result, std::string const& fieldName) const
{
    auto mappingIt = m_componentMapping.find(fieldName);
    if (mappingIt == m_componentMapping.end()) {
        return QVariant();
    }
    std::string const& realFieldName = mappingIt->second;
    if (!result->contains(realFieldName)) {
        return QVariant();
    }
    scopes::Variant const& v = result->value(realFieldName);
    if (v.which() != scopes::Variant::Type::String) {
        return QVariant();
    }

    return QString::fromStdString(v.get_string());
}

QVariant
ResultsModel::attributesValue(scopes::CategorisedResult const* result) const
{
    auto mappingIt = m_componentMapping.find("attributes");
    if (mappingIt == m_componentMapping.end()) {
        return QVariant();
    }
    std::string const& realFieldName = mappingIt->second;
    if (!result->contains(realFieldName)) {
        return QVariant();
    }
    scopes::Variant const& v = result->value(realFieldName);
    if (v.which() != scopes::Variant::Type::Array) {
        return QVariant();
    }

    QVariantList attributes;
    scopes::VariantArray arr(v.get_array());
    for (unsigned i = 0; i < arr.size(); i++) {
        if (arr[i].which() != scopes::Variant::Type::Dict) {
            continue;
        }
        QVariantMap attribute(scopeVariantToQVariant(arr[i]).toMap());
        attributes << QVariant(attribute);
        // we'll limit the number of attributes
        if (attributes.size() >= m_maxAttributes) {
            break;
        }
    }

    return attributes;
}

QHash<int, QByteArray> ResultsModel::roleNames() const
{
    QHash<int, QByteArray> roles(unity::shell::scopes::ResultsModelInterface::roleNames());
    roles.insert(ExtraRoles::RoleScopeId, "scopeId");

    return roles;
}

QVariant
ResultsModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (row >= m_results.size())
    {
        qWarning() << "ResultsModel::data - invalid index" << row << "size"
                << m_results.size();
        return QVariant();
    }

    scopes::CategorisedResult* result = m_results.at(index.row()).get();

    switch (role) {
        case RoleUri:
            return QString::fromStdString(result->uri());
        case RoleCategoryId:
            return QString::fromStdString(result->category()->id());
        case RoleDndUri:
            return QString::fromStdString(result->dnd_uri());
        case RoleResult:
            return QVariant::fromValue(std::static_pointer_cast<unity::scopes::Result>(m_results.at(index.row())));
        case RoleTitle:
            return componentValue(result, "title");
        case RoleArt: {
            QString image(componentValue(result, "art").toString());
            if (image.isEmpty()) {
                QString uri(QString::fromStdString(result->uri()));
                // FIXME: figure out a better way and get rid of this, it's an awful hack
                QVariantHash result_meta;
                if (result->contains("artist") && result->contains("album")) {
                    result_meta["artist"] = scopeVariantToQVariant(result->value("artist"));
                    result_meta["album"] = scopeVariantToQVariant(result->value("album"));
                }
                QString thumbnailerUri(uriToThumbnailerProviderString(uri, result_meta));
                if (!thumbnailerUri.isNull()) {
                    return thumbnailerUri;
                }
            }
            return image;
        }
        case RoleSubtitle:
            return componentValue(result, "subtitle");
        case RoleMascot:
            return componentValue(result, "mascot");
        case RoleEmblem:
            return componentValue(result, "emblem");
        case RoleAttributes:
            return attributesValue(result);
        case RoleSummary:
            return componentValue(result, "summary");
        case RoleBackground: {
            QVariant backgroundVariant(componentValue(result, "background"));
            if (backgroundVariant.isNull()) {
                return backgroundVariant;
            }
            return backgroundUriToVariant(backgroundVariant.toString());
        }
        case RoleOverlayColor:
            return componentValue(result, "overlay-color");
        case RoleScopeId:
            if (result->uri().compare(0, 8, "scope://") == 0) {
                try {
                    scopes::CannedQuery q(scopes::CannedQuery::from_uri(result->uri()));
                    return QString::fromStdString(q.scope_id());
                } catch (...) {
                    // silently ignore and return "undefined"
                }
            }
            return QVariant();
        default:
            return QVariant();
    }
}

} // namespace scopes_ng
