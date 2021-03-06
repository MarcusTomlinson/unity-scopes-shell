/*
 * Copyright (C) 2012 Canonical, Ltd.
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
 *
 * Author: Michał Sawicz <michal.sawicz@canonical.com>
 */

// Qt
#include <QQmlContext>
#include <qqml.h>

// self
#include "plugin.h"

// local
#include "scopes.h"
#include "scope.h"
#include "categories.h"
#include "department.h"
#include "resultsmodel.h"
#include "previewmodel.h"
#include "previewwidgetmodel.h"
#include "settingsmodel.h"
#include <unity/shell/scopes/FiltersInterface.h>
#include <unity/shell/scopes/OptionSelectorFilterInterface.h>
#include <unity/shell/scopes/OptionSelectorOptionsInterface.h>
#include <unity/shell/scopes/RangeInputFilterInterface.h>
#include <unity/shell/scopes/ValueSliderFilterInterface.h>
#include <unity/shell/scopes/ValueSliderValuesInterface.h>
#include "localization.h"

void UnityPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Unity"));

    init_gettext_domain();

    // new Scopes classes
    qmlRegisterType<scopes_ng::Scopes>(uri, 0, 2, "Scopes");
    qmlRegisterUncreatableType<unity::shell::scopes::ScopeInterface>(uri, 0, 2, "Scope", QStringLiteral("Can't create Scope object in QML. Get them from Scopes instance."));
    qmlRegisterUncreatableType<unity::shell::scopes::NavigationInterface>(uri, 0, 2, "Navigation", QStringLiteral("Can't create Navigation object in QML. Get them from Scope instance."));
    qmlRegisterUncreatableType<unity::shell::scopes::CategoriesInterface>(uri, 0, 2, "Categories", QStringLiteral("Can't create Categories object in QML. Get them from Scope instance."));
    qmlRegisterUncreatableType<unity::shell::scopes::SettingsModelInterface>(uri, 0, 2, "Settings", QStringLiteral("Can't create Settings object in QML. Get them from Scope instance."));
    qmlRegisterUncreatableType<scopes_ng::ResultsModel>(uri, 0, 2, "ResultsModel", QStringLiteral("Can't create new ResultsModel in QML. Get them from Categories instance."));
    qmlRegisterUncreatableType<unity::shell::scopes::PreviewModelInterface>(uri, 0, 2, "PreviewModel", QStringLiteral("Can't create new PreviewModel in QML. Get them from Scope instance."));
    qmlRegisterUncreatableType<scopes_ng::PreviewWidgetModel>(uri, 0, 2, "PreviewWidgetModel", QStringLiteral("Can't create new PreviewWidgetModel in QML. Get them from PreviewModel instance."));
    qmlRegisterUncreatableType<unity::shell::scopes::FiltersInterface>(uri, 0, 2, "Filters", "Can't create Filters object in QML. Get them from Scope instance.");
    qmlRegisterUncreatableType<unity::shell::scopes::FilterBaseInterface>(uri, 0, 2, "Filter", "Can't create Filter object in QML. Get them from Scope instance.");
    qmlRegisterUncreatableType<unity::shell::scopes::OptionSelectorOptionsInterface>(uri, 0, 2, "OptionSelectorOptions", "Can't create Filters object in QML. Get them from OptionSelector instance.");
    qmlRegisterUncreatableType<unity::shell::scopes::RangeInputFilterInterface>(uri, 0, 2, "RangeInputFilter", "Can't create new RangeInputFilter in QML. Get them from Filters instance.");
    qmlRegisterUncreatableType<unity::shell::scopes::ValueSliderFilterInterface>(uri, 0, 2, "ValueSliderFilter", "Can't create new ValueSliderFilter in QML. Get them from Filters instance.");
    qmlRegisterUncreatableType<unity::shell::scopes::ValueSliderValuesInterface>(uri, 0, 2, "ValueSliderValues", "Can't create new ValueSliderValues in QML. Get them from ValueSlideriFilter instance.");
}

void UnityPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
