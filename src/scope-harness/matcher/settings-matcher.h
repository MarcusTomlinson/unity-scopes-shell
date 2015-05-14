/*
* Copyright (C) 2015 Canonical, Ltd.
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of version 3 of the GNU Lesser General Public License as published
* by the Free Software Foundation.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Author: Pawel Stolowski <pawel.stolowski@canonical.com>
*/

#pragma once

#include <QtGlobal>
#include <memory>

#include <scope-harness/matcher/match-result.h>

namespace unity
{
namespace scopeharness
{

namespace view
{
class SettingsView;
}

namespace matcher
{

class SettingsOptionMatcher;

class Q_DECL_EXPORT SettingsMatcher final
{
public:
    SettingsMatcher();
    SettingsMatcher& option(const SettingsOptionMatcher& optionMatcher);
    SettingsMatcher& hasAtLeast(std::size_t minimum);
    SettingsMatcher& hasExactly(std::size_t amount);

    MatchResult match(const view::SettingsView& settings);
    void match(MatchResult& matchResult, const view::SettingsView& settings);

protected:
    struct _Priv;
    std::shared_ptr<_Priv> p;
};

}
}
}
