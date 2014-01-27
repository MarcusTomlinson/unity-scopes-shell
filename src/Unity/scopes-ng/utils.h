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

#ifndef NG_UTILS_H
#define NG_UTILS_H

// Qt
#include <QVariant>

#include <unity/scopes/Result.h>
#include <unity/scopes/Variant.h>

namespace scopes_ng
{

QVariant scopeVariantToQVariant(unity::scopes::Variant const& variant);

} // namespace scopes_ng

#endif // NG_SCOPES_H
