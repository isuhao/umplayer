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

#ifndef _PREFPERFORMANCE_H_
#define _PREFPERFORMANCE_H_

#include "ui_prefperformance.h"
#include "prefwidget.h"

#include "preferences.h"
#include "config.h"

class PrefPerformance : public PrefWidget, public Ui::PrefPerformance
{
	Q_OBJECT

public:
        enum Format
        {
            MP4 = 0,
            FLV = 1
        };
	PrefPerformance( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~PrefPerformance();

	virtual QString sectionName();
	virtual QPixmap sectionIcon();

    // Pass data to the dialog
    void setData(Preferences * pref);

    // Apply changes
    void getData(Preferences * pref);

protected:
	virtual void createHelp();

	void setCacheForFiles(int n);
	int cacheForFiles();

	void setCacheForHTTPStreams(int n);
        void setCacheForOtherStreams(int n);
        int cacheForHTTPStreams();
        int cacheForOtherStreams();

	void setCacheForDVDs(int n);
	int cacheForDVDs();

	void setCacheForAudioCDs(int n);
	int cacheForAudioCDs();

	void setCacheForVCDs(int n);
	int cacheForVCDs();

	void setCacheForTV(int n);
	int cacheForTV();

	void setPriority(int n);
	int priority();

	void setFrameDrop(bool b);
	bool frameDrop();

	void setHardFrameDrop(bool b);
	bool hardFrameDrop();

	void setSkipLoop(Preferences::H264LoopFilter value);
	Preferences::H264LoopFilter skipLoop();

#if !SMART_DVD_CHAPTERS
	void setFastChapterSeeking(bool b);
	bool fastChapterSeeking();
#endif

	void setFastAudioSwitching(Preferences::OptionState value);
	Preferences::OptionState fastAudioSwitching();

	void setThreads(int v);
	int threads();

	void setCoreavcUsage(bool b);
	bool coreavcUsage();

protected:
	virtual void retranslateStrings();

};

#endif
