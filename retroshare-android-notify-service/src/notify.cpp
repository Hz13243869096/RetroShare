/*
 * RetroShare Android Service
 * Copyright (C) 2016  Gioacchino Mazzurco <gio@eigenlab.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFileInfo>
#include <QDebug>

#ifdef __ANDROID__
#	include "util/androiddebug.h"
#endif

#include "libresapilocalclient.h"
#include "retroshare/rsinit.h"


int main(int argc, char *argv[])
{
#ifdef __ANDROID__
	AndroidStdIOCatcher dbg; (void) dbg;
#endif

	QCoreApplication app(argc, argv);

	QString sockPath = QString::fromStdString(RsAccounts::ConfigDirectory());
	sockPath.append("/libresapi.sock");

	QQmlApplicationEngine engine;
	qmlRegisterType<LibresapiLocalClient>(
	            "org.retroshare.qml_components.LibresapiLocalClient", 1, 0,
	            "LibresapiLocalClient");

#ifdef QT_DEBUG
	engine.rootContext()->setContextProperty("QT_DEBUG", true);
#else
	engine.rootContext()->setContextProperty("QT_DEBUG", false);
#endif // QT_DEBUG

	engine.rootContext()->setContextProperty("apiSocketPath", sockPath);

	LibresapiLocalClient rsApi;

	while (!QFileInfo::exists(sockPath))
	{
		qDebug() << "RetroShareAndroidNotifyService waiting for core to"
		         << "listen on:" << sockPath;

		usleep(2500000);
	}

	rsApi.openConnection(sockPath);

	engine.rootContext()->setContextProperty("rsApi", &rsApi);
	engine.load(QUrl(QLatin1String("qrc:/notify.qml")));

	return app.exec();
}
