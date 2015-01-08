/*
 * Copyright (C) 2014 Canonical, Ltd.
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

#include <boost/python.hpp>
#include <scope-harness/results/category.h>

using namespace boost::python;
namespace shr = unity::scopeharness::results;

static PyObject* getResultsList(const shr::Category& cat)
{
    list pylist;
    for (auto const res: cat.results())
    {
        pylist.append(res);
    }
    return incref(pylist.ptr());
}

void export_category()
{
    shr::Result (shr::Category::*result_by_uri)(const std::string&) const = &shr::Category::result;
    shr::Result (shr::Category::*result_by_index)(std::size_t) const = &shr::Category::result;

    class_<shr::Category>("Category", no_init)
        .add_property("id", &shr::Category::id)
        .add_property("title", &shr::Category::title)
        .add_property("icon", &shr::Category::icon)
        .add_property("header_link", &shr::Category::headerLink)
        .add_property("renderer", &shr::Category::renderer)
        .add_property("components", &shr::Category::components)
        .add_property("results", &getResultsList)
        .def("result", result_by_uri, return_value_policy<return_by_value>())
        .def("result", result_by_index, return_value_policy<return_by_value>())
        ;
}
