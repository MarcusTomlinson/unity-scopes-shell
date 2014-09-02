/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *  Pete Woods <pete.woods@canonical.com>
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

#include "settingsmodel.h"
#include "utils.h"

#include <QDebug>
#include <QDir>
#include <QTimer>

using namespace scopes_ng;
namespace sc = unity::scopes;

SettingsModel::SettingsModel(const QDir& configDir, const QString& scopeId,
        const QVariant& settingsDefinitions, QObject* parent,
        int settingsTimeout)
        : SettingsModelInterface(parent), m_settingsTimeout(settingsTimeout)
{
    configDir.mkpath(scopeId);
    QDir databaseDir = configDir.filePath(scopeId);

    m_settings.reset(new QSettings(databaseDir.filePath("settings.ini"), QSettings::IniFormat));

    for (const auto &it : settingsDefinitions.toList())
    {
        QVariantMap data = it.toMap();
        QString id = data["id"].toString();
        QString displayName = data["displayName"].toString();
        QVariantMap properties;
        QVariant defaultValue;
        if (data.contains("displayValues"))
        {
            properties["values"] = data["displayValues"].toList();
        }
        QString type = data["type"].toString();

        QVariant::Type variantType;

        if(type == "boolean")
        {
            variantType = QVariant::Bool;
        }
        else if(type == "list")
        {
            variantType = QVariant::UInt;
        }
        else if(type == "number")
        {
            variantType = QVariant::Double;
        }
        else if(type == "string")
        {
            variantType = QVariant::String;
        }

        if(data.contains("defaultValue"))
        {
            defaultValue = data["defaultValue"];
            properties["defaultValue"] = defaultValue;
        }

        QSharedPointer<QTimer> timer(new QTimer());
        timer->setProperty("setting_id", id);
        timer->setSingleShot(true);
        timer->setInterval(m_settingsTimeout);
        timer->setTimerType(Qt::VeryCoarseTimer);
        connect(timer.data(), SIGNAL(timeout()), this,
                SLOT(settings_timeout()));
        m_timers[id] = timer;

        QSharedPointer<Data> setting(
                new Data(id, displayName, type, properties, defaultValue,
                        variantType));

        m_data << setting;
        m_data_by_id[id] = setting;
    }
}

QVariant SettingsModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    QVariant result;

    if (row < m_data.size())
    {
        auto data = m_data[row];

        switch (role)
        {
            case Roles::RoleSettingId:
                result = data->id;
                break;
            case Roles::RoleDisplayName:
                result = data->displayName;
                break;
            case Roles::RoleType:
                result = data->type;
                break;
            case Roles::RoleProperties:
                result = data->properties;
                break;
            case Roles::RoleValue:
            {
                result = m_settings->value(data->id, data->defaultValue);
                result.convert(data->variantType);
                break;
            }
            default:
                break;
        }
    }

    return result;
}

QVariant SettingsModel::value(const QString& id) const
{
    m_settings->sync();

    QVariant result;

    QSharedPointer<Data> data = m_data_by_id[id];
    if (data)
    {
        result = m_settings->value(data->id, data->defaultValue);
        result.convert(data->variantType);
    }
    return result;
}

bool SettingsModel::setData(const QModelIndex &index, const QVariant &value,
        int role)
{
    int row = index.row();
    QVariant result;

    if (row < m_data.size())
    {
        auto data = m_data[row];

        switch (role)
        {
            case Roles::RoleValue:
            {
                QSharedPointer<QTimer> timer = m_timers[data->id];
                timer->setProperty("value", value);
                timer->start();

                return true;
            }
            default:
                break;
        }
    }

    return false;
}

int SettingsModel::rowCount(const QModelIndex&) const
{
    return count();
}

int SettingsModel::count() const
{
    return m_data.size();
}

void SettingsModel::settings_timeout()
{
    QObject *timer = sender();
    if (!timer)
    {
        return;
    }

    QString setting_id = timer->property("setting_id").toString();
    QVariant value = timer->property("value");

    m_settings->setValue(setting_id, value);
}