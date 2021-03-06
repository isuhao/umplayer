/*  umplayer, GUI front-end for mplayer.
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

#ifndef YTDIALOG_H
#define YTDIALOG_H

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QMovie>
#include <QMap>
#include <QPushButton>
#include <QLineEdit>


class YTTabBar;
class YTDelegate;
class SingleVideoItem;
class YTReply;
class YTDataAPI;
class SearchBox;
class QNetworkReply;
class QNetworkAccessManager;
class QTimeLine;

class OverlayWidget : public QWidget
{
    Q_OBJECT
public:
    OverlayWidget(QWidget* parent = 0);
    void setText(QString text);
    QString text() { return m_text; }
protected:
    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *s);
    void hideEvent(QHideEvent *h);
private:
    QPixmap loadingOverlay;    
    QTimeLine* timeLine;
    QPixmap exclaimPix;
    QString m_text;
    QPixmap stages[12];

private slots:
    void frameHasChanged(int frame);

};

class YTButton : public QAbstractButton
{
    Q_OBJECT
public:
    YTButton(QWidget* parent = 0);
    void setPix(QPixmap pix);

protected:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    QPixmap normalPix;
    QPixmap hoverPix;
    bool hovered;


};

class PixmapLoader : public QObject
{
    Q_OBJECT
public:
    static PixmapLoader* instance();
    int getPixmap(QString url);
    void reset();
    ~PixmapLoader();
public slots:
    void gotPixmap(QNetworkReply* reply);

signals:
    void pixmapResult(QPixmap, int);

private:
    PixmapLoader(QObject* parent = 0);

    QNetworkAccessManager* manager;
    static PixmapLoader* m_instance;

};


class YTDialog : public QWidget
{
Q_OBJECT
public:
    enum ItemDataRole
    {
        Hovered = Qt::UserRole + 1,
        Clicked = Qt::UserRole + 2
    };
    enum Quality
    {
        FullHD = 37,
        HD = 22,
        Normal = 18,
        NormalFlv = 35,
        BelowNormalFlv = 34,
        LowFlv = 5
    };
    enum Tabs
    {
        Relevant,
        Recent,
        Viewed,

        Popular,
        ViewedButton,
        Rated
    };

    enum Mode
    {
        Button,
        Search
    };

    struct TabData
    {
        TabData()
        {
            pageNo = 1;
            populated = false;
        }
        int pageNo;
        QList<SingleVideoItem*> data;
        bool populated;
        QString nextUrl;
    };

    static YTDialog* instance();
    ~YTDialog() { delete PixmapLoader::instance();}
    void setLoadingOverlay(bool enable);    
    bool eventFilter(QObject *w, QEvent *e);    
    void setMode(Mode mode);
    void addTab(Tabs tab);    



signals:
    void gotUrls(const QMap<int, QString>&, QString, QString);

public slots:
    void gotCurrentTab(int index);
    void nextClicked();
    void prevClicked();
    void gotAPIReply(const YTReply&);
    void gotPixmap(QPixmap pix, int id);
    void videoClicked(QListWidgetItem* item);
    void videoDblClicked(QListWidgetItem* item);
    void setSearchTerm(QString term);
    void showContextMenu(QPoint point);
    void recordItem(QListWidgetItem* item);


private:
    static YTDialog* m_instance;
    YTTabBar* tabBar;
    YTDataAPI* api;
    QListWidget* videoList;
    YTDelegate* delegate;
    bool overlayVisible;
    OverlayWidget* overlay;
    QMap<Tabs, TabData> entries;
    QPushButton* nextButton;
    QPushButton* prevButton;
    QMap<int, Tabs> resultForTab;
    QMap<int, SingleVideoItem*> pendingPixmapQueue;
    QString searchTerm;
    SearchBox* searchBox;

    explicit YTDialog(QWidget *parent = 0);    
    void updateNextPrevWidget();    
    void reset();
    int lastPageNo(Tabs tab);


protected:
    void resizeEvent(QResizeEvent *r);


};

#endif // YTDIALOG_H
