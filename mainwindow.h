/*
 * mainwindow.h - header file for MainWindow class
 *
 * Copyright (c) 2009 Tobias Doerffel / Electronic Design Chemnitz
 *
 * This file is part of QDataMatrix
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QImage>
#include <QtGui/QMainWindow>

//#include "ui_about.h"

class QGraphicsScene;
class QGraphicsPixmapItem;


namespace Ui
{
    class QDataMatrix;
}


class QLabel;


/*
class AboutDialog : public QDialog, public Ui::AboutDialog
{
public:
	AboutDialog( QWidget * _parent ) :
		QDialog( _parent )
	{
		setupUi( this );
		aboutTextLabel->setText(
			aboutTextLabel->text().arg( "0.1.0" ) );
	}
} ;
*/

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow( QWidget * parent = 0 );
	~MainWindow();

	QImage renderPage( bool _no_margin = false );


protected:
	virtual void resizeEvent( QResizeEvent * _ev );


private slots:
	void openSettings( void );
	void saveSettings( void );
	void rerenderAndUpdatePreview();
	void updatePreview();
	void save();
	void print();
	void updateAutoZoom();
	void updateZoom();


private:
	void loadSettingsFile( const QString & _file );
	int convertToPx( int _v );
	void print( QPrinter * _target );

	QGraphicsScene * m_scene;
	QGraphicsPixmapItem * m_previewItem;
	QImage m_cachedPreview;
	Ui::QDataMatrix * ui;

};

#endif // MAINWINDOW_H
