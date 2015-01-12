/*  umplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2009 Ricardo Villalba <rvm@escomposlinux.org>
    Copyright (C) 2010 Ori Rejwan

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <QApplication>
#include <QFile>
#include <QTime>
#include <QTextCodec>

#include "umplayer.h"
#include "constants.h"
#include "global.h"
#include "helper.h"
#include "paths.h"


#ifdef Q_OS_WIN32
#include "AppUserModel.h"
#endif

#include <stdio.h>

#define USE_LOCKS 1
#if USE_LOCKS
#ifdef USE_QXT
#define USE_QXT_LOCKS 1
#endif // USE_QXT
#endif // USE_LOCKS

#if USE_LOCKS && USE_QXT_LOCKS
#include <QxtFileLock>
#endif

using namespace Global;

BaseGui * basegui_instance = 0;

void myMessageOutput( QtMsgType type, const char *msg ) {
	static QStringList saved_lines;
	static QString orig_line;
	static QString line2;
	static QRegExp rx_log;

	if (pref) {
                if (!pref->log_umplayer) return;
		rx_log.setPattern(pref->log_filter);
	} else {
		rx_log.setPattern(".*");
	}

	line2.clear();

	orig_line = QString::fromUtf8(msg);

	switch ( type ) {
		case QtDebugMsg:
			if (rx_log.indexIn(orig_line) > -1) {
				#ifndef NO_DEBUG_ON_CONSOLE
                                fprintf( stderr, "Debug: %s\n", orig_line.toUtf8().data() );
				#endif
				line2 = orig_line;
			}
			break;
		case QtWarningMsg:
			#ifndef NO_DEBUG_ON_CONSOLE
                        fprintf( stderr, "Warning: %s\n", orig_line.toUtf8().data() );
			#endif
			line2 = "WARNING: " + orig_line;
			break;
		case QtFatalMsg:
			#ifndef NO_DEBUG_ON_CONSOLE
                        fprintf( stderr, "Fatal: %s\n", orig_line.toUtf8().data() );
			#endif
			line2 = "FATAL: " + orig_line;
			abort();                    // deliberately core dump
		case QtCriticalMsg:
			#ifndef NO_DEBUG_ON_CONSOLE
                        fprintf( stderr, "Critical: %s\n", orig_line.toUtf8().data() );
			#endif
			line2 = "CRITICAL: " + orig_line;
			break;
	}

	if (line2.isEmpty()) return;

	line2 = "["+ QTime::currentTime().toString("hh:mm:ss:zzz") +"] "+ line2;

	if (basegui_instance) {
		if (!saved_lines.isEmpty()) {
			// Send saved lines first
			for (int n=0; n < saved_lines.count(); n++) {
                                basegui_instance->recordUmplayerLog(saved_lines[n]);
			}
			saved_lines.clear();
		}
                basegui_instance->recordUmplayerLog(line2);
	} else {
		// GUI is not created yet, save lines for later
		saved_lines.append(line2);
	}
}

#if USE_LOCKS
#if USE_QXT_LOCKS
bool create_lock(QFile * f, QxtFileLock * lock) {
	bool success = false;
	if (f->open(QIODevice::ReadWrite)) {
	 	if (lock->lock()) {
                        f->write("umplayer lock file");
			success = true;
		}
	}
	if (!success) f->close();
	return success;
}

void clean_lock(QFile * f, QxtFileLock * lock) {
	qDebug("main: clean_lock: %s", f->fileName().toUtf8().data());
	lock->unlock();
	f->close();
	f->remove();
}
#else
void remove_lock(QString lock_file) {
	if (QFile::exists(lock_file)) {
		qDebug("main: remove_lock: %s", lock_file.toUtf8().data());
		QFile::remove(lock_file);
	}
}
#endif
#endif


int main( int argc, char ** argv ) 
{
	MyApplication a( argc, argv );
	a.setQuitOnLastWindowClosed(false);
	//a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );

	// Sets the config path
	QString config_path;        
#ifdef Q_OS_WIN32
        if(QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
        {
            QW7::AppUserModel::SetCurrentProcessExplicitAppUserModelID( APP_ID );
        }
#endif
#ifdef PORTABLE_APP
	config_path = a.applicationDirPath();
#else
        // If a umplayer.ini exists in the app path, will use that path
	// for the config file by default
        if (QFile::exists( a.applicationDirPath() + "/umplayer.ini" ) ) {
		config_path = a.applicationDirPath();
                qDebug("main: using existing %s", QString(config_path + "/umplayer.ini").toUtf8().data());
	}
#endif

	QStringList args = a.arguments();
	int pos = args.indexOf("-config-path");
	if ( pos != -1) {
		if (pos+1 < args.count()) {
			pos++;
			config_path = args[pos];
			// Delete from list
			args.removeAt(pos);
			args.removeAt(pos-1);
		} else {
			printf("Error: expected parameter for -config-path\r\n");
			return UMPlayer::ErrorArgument;
		}
	}

    qInstallMsgHandler( myMessageOutput );

#if USE_LOCKS
	//setIniPath will be set later in global_init, but we need it here
	if (!config_path.isEmpty()) Paths::setConfigPath(config_path);

        QString lock_file = Paths::iniPath() + "/umplayer_init.lock";
	qDebug("main: lock_file: %s", lock_file.toUtf8().data());

#if USE_QXT_LOCKS
	QFile f(lock_file);
	QxtFileLock write_lock(&f, 0x10, 30, QxtFileLock::WriteLock);

	bool lock_ok = create_lock(&f, &write_lock);

	if (!lock_ok) {
		//lock failed
		qDebug("main: lock failed");

		// Wait 10 secs max.
		int n = 100;
		while ( n > 0) {
			Helper::msleep(100); // wait 100 ms
			//if (!QFile::exists(lock_file)) break;
			if (create_lock(&f, &write_lock)) break;
			n--;
			if ((n % 10) == 0) qDebug("main: waiting %d...", n);
		}
		// Continue startup
	}
#else
	if (QFile::exists(lock_file)) {
		qDebug("main: %s exists, waiting...", lock_file.toUtf8().data());
		// Wait 10 secs max.
		int n = 100;
		while ( n > 0) {
			Helper::msleep(100); // wait 100 ms
			if (!QFile::exists(lock_file)) break;
			n--;
			if ((n % 10) == 0) qDebug("main: waiting %d...", n);
		}
		remove_lock(lock_file);
	} else {
		// Create lock file
		QFile f(lock_file);
		if (f.open(QIODevice::WriteOnly)) {
                        f.write("umplayer lock file");
			f.close();
		} else {
			qWarning("main: can't open %s for writing", lock_file.toUtf8().data());
		}
		
	}
#endif // USE_QXT_LOCKS
#endif // USE_LOCKS

        UMPlayer * umplayer = new UMPlayer(config_path);
        umplayer->setBasegui_instance(&basegui_instance);
        UMPlayer::ExitCode c = umplayer->processArgs( args );
	if (c != UMPlayer::NoExit) {
#if USE_LOCKS
#if USE_QXT_LOCKS
		clean_lock(&f, &write_lock);
#else
		remove_lock(lock_file);
#endif
#endif
		return c;
	}
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
        basegui_instance = umplayer->gui();
        a.connect(umplayer->gui(), SIGNAL(quitSolicited()), &a, SLOT(quit()));
        umplayer->start();


#if USE_LOCKS
#if USE_QXT_LOCKS
	clean_lock(&f, &write_lock);
#else
	remove_lock(lock_file);
#endif
#endif

	int r = a.exec();

	basegui_instance = 0;
        delete umplayer;


	return r;
}

