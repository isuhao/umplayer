/*  umplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2009 Ricardo Villalba <rvm@escomposlinux.org>

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

#ifndef _SUBCHOOSERDIALOG_H_
#define _SUBCHOOSERDIALOG_H_

#include <QDialog>
#include "ui_subchooserdialog.h"

class SubChooserDialog : public QDialog, public Ui::SubChooserDialog
{
	Q_OBJECT

public:
	SubChooserDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~SubChooserDialog();

	void addFile(QString filename); 
	QStringList selectedFiles();

protected:
	virtual void retranslateStrings();
    virtual void changeEvent ( QEvent * event ) ;

protected slots:
	void selectAllClicked(bool); 
	void selectNoneClicked(bool); 
	void listItemClicked(QListWidgetItem* item); 
	void listItemPressed(QListWidgetItem* item); 
};

#endif
