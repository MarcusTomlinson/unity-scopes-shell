/*
 * Copyright (C) 2013-2014 Canonical, Ltd.
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
 * Authors:
 *  Pete Woods <pete.woods@canonical.com>
 */

#include "ubuntulocationservice.h"

#include <QProcess>
#include <QSignalSpy>
#include <QTest>
#include <string>

#include <libqtdbusmock/DBusMock.h>
#include <libqtdbustest/DBusTestRunner.h>

using namespace std;
using namespace scopes_ng;
using namespace unity::scopes;
using namespace QtDBusTest;
using namespace QtDBusMock;

namespace
{

static const char* LOCATION_SERVICE_NAME = "com.ubuntu.location.Service";
static const char* LOCATION_SERVICE_PATH = "/com/ubuntu/location/Service";
static const char* LOCATION_SERVICE_INTERFACE = "com.ubuntu.location.Service";

static const char* SESSION_NAME = "com.ubuntu.location.Service.Session";
static const QString SESSION_PATH = "/com/ubuntu/location/session/%1";
static const char* SESSION_INTERFACE = "com.ubuntu.location.Service.Session";

static const string GEOIP_JSON = R"(
{
  "areaCode":"0",
  "city":"Accrington",
  "countryCode":"GB",
  "countryName":"United Kingdom",
    "position": {
      "accuracy": {
      "horizontal":100000.0
    },
    "latitude":55.76540,
    "longitude":-2.74670
  },
  "regionCode":"H2",
  "regionName":"Lancashire",
  "zipPostalCode":"BB5"
}
)";

class LocationTest: public QObject
{
Q_OBJECT

public:
    LocationTest() :
            mock(dbus)
    {
    }

private:
    DBusTestRunner dbus;

    DBusMock mock;

    QScopedPointer<LocationService> locationService;

    QProcess geoIpServer;

    QUrl url;

private Q_SLOTS:
    void initTestCase()
    {
        DBusMock::registerMetaTypes();

        // Register the main interface object
        mock.registerCustomMock(LOCATION_SERVICE_NAME, LOCATION_SERVICE_PATH,
                                LOCATION_SERVICE_INTERFACE,
                                QDBusConnection::SystemBus);

        // Register the first session object
        mock.registerCustomMock(SESSION_NAME, SESSION_PATH.arg(0),
                                SESSION_INTERFACE,
                                QDBusConnection::SystemBus);

        dbus.startServices();

        // Set up the main interface
        {
            OrgFreedesktopDBusMockInterface& interface = mock.mockInterface(
                    LOCATION_SERVICE_NAME, LOCATION_SERVICE_PATH,
                    LOCATION_SERVICE_INTERFACE, QDBusConnection::SystemBus);

            interface.AddMethod(LOCATION_SERVICE_INTERFACE,
                                "CreateSessionForCriteria", "bbbbdbbb", "o",
                                "ret='/com/ubuntu/location/session/0'");
        }

        // Set up the first session
        {
            OrgFreedesktopDBusMockInterface& session = mock.mockInterface(
                    SESSION_NAME, SESSION_PATH.arg(0), SESSION_INTERFACE,
                    QDBusConnection::SystemBus);

            session.AddMethod(SESSION_INTERFACE, "StartPositionUpdates", "", "",
                              "");
        }

        geoIpServer.setProcessChannelMode(QProcess::ForwardedErrorChannel);
        geoIpServer.start(GEOIP_SERVER_BINARY);
        QVERIFY(geoIpServer.waitForStarted());
        QVERIFY(geoIpServer.waitForReadyRead());

        url = "http://127.0.0.1:" + geoIpServer.readAllStandardOutput().trimmed() + "/lookup";
    }

    void cleanupTestCase() {
        geoIpServer.terminate();
        QVERIFY(geoIpServer.waitForFinished());
    }

    void init()
    {
        locationService.reset(new UbuntuLocationService(GeoIp::Ptr(new GeoIp(url))));
    }

    void testGeoIpLocation()
    {
        QSignalSpy spy(locationService.data(), SIGNAL(locationChanged()));
        locationService->activate();

        QVERIFY(spy.wait());
        QCOMPARE(Variant::deserialize_json(GEOIP_JSON), locationService->location());
    }
};

}
QTEST_GUILESS_MAIN(LocationTest)
#include <locationtest.moc>
