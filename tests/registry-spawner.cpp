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
 *  Michal Hruby <michal.hruby@canonical.com>
 */

#include "registry-spawner.h"

#include <QTest>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QThread>
#include <QSignalSpy>

#define SCOPES_TMP_ENDPOINT_DIR "/tmp/scopes-test-endpoints"

RegistrySpawner::RegistrySpawner()
{
    qputenv("UNITY_SCOPES_CONFIG_DIR", m_tempDir.path().toUtf8());
    qputenv("TEST_DESKTOP_FILES_DIR", "");

    QDir endpointdir(QFileInfo(TEST_RUNTIME_CONFIG).dir());
    endpointdir.cd(QString("endpoints"));
    QFile::remove(SCOPES_TMP_ENDPOINT_DIR);
    // symlinking to workaround "File name too long" issue
    QVERIFY2(QFile::link(endpointdir.absolutePath(), SCOPES_TMP_ENDPOINT_DIR),
        "Unable to create symlink " SCOPES_TMP_ENDPOINT_DIR);
    // startup our private scope registry
    QString registryBin(TEST_SCOPEREGISTRY_BIN);
    QStringList arguments;
    arguments << TEST_RUNTIME_CONFIG;

    m_registry.reset(new QProcess(nullptr));
    m_registry->setProcessChannelMode(QProcess::ForwardedChannels);
    m_registry->start(registryBin, arguments);
    QVERIFY(m_registry->waitForStarted());

    qputenv("UNITY_SCOPES_TYPING_TIMEOUT_OVERRIDE", "0");
    qputenv("UNITY_SCOPES_LIST_DELAY", "5");
    qputenv("UNITY_SCOPES_RESULTS_TTL_OVERRIDE", "250");
    qputenv("UNITY_SCOPES_RUNTIME_PATH", TEST_RUNTIME_CONFIG);
    qputenv("UNITY_SCOPES_NO_LOCATION", "1");
}

RegistrySpawner::~RegistrySpawner()
{
    if (m_registry) {
        m_registry->terminate();
        if (!m_registry->waitForFinished()) {
            m_registry->kill();
        }
    }
    QFile::remove(SCOPES_TMP_ENDPOINT_DIR);
}