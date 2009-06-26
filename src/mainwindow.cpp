/*
 * mainwindow.cpp - implementation of MainWindow class
 *
 * Copyright (c) 2009 Tobias Doerffel / Electronic Design Chemnitz
 *
 * This file is part of QDataMatrix - http://qdatamatrix.sourceforge.net
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

#include <QtCore/QSettings>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtGui/QFileDialog>
#include <QtGui/QFont>
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QtGui/QScrollBar>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <dmtx.h>



MainWindow::MainWindow( QWidget * _parent ) :
	QMainWindow( _parent ),
	ui( new Ui::QDataMatrix )
{
	ui->setupUi( this );

	foreach( QLineEdit * l, findChildren<QLineEdit *>() )
	{
		connect( l, SIGNAL( editingFinished() ),
				this, SLOT( rerenderAndUpdatePreview() ) );
	}
	foreach( QSpinBox * s, findChildren<QSpinBox *>() )
	{
		connect( s, SIGNAL( valueChanged( int ) ),
			this, SLOT( rerenderAndUpdatePreview() ) );
	}

	connect( ui->actionOpen, SIGNAL( triggered() ),
			this, SLOT( openSettings() ) );

	connect( ui->actionSave, SIGNAL( triggered() ),
			this, SLOT( saveSettings() ) );
	connect( ui->actionAbout, SIGNAL( triggered() ),
			this, SLOT( about() ) );
	connect( ui->barcodeScale, SIGNAL( valueChanged( double ) ),
			this, SLOT( rerenderAndUpdatePreview() ) );

	connect( ui->zoomSlider, SIGNAL( valueChanged( int ) ),
			this, SLOT( updateZoom() ) );

	connect( ui->autoZoom, SIGNAL( toggled( bool ) ),
			this, SLOT( updateAutoZoom() ) );

	connect( ui->saveButton, SIGNAL( clicked() ),
			this, SLOT( save() ) );
	connect( ui->printButton, SIGNAL( clicked() ),
			this, SLOT( print() ) );

	m_scene = new QGraphicsScene;
	m_scene->setBackgroundBrush( QColor( 128, 128, 128 ) );
	m_previewItem = m_scene->addPixmap( QPixmap() );
	ui->previewArea->setScene( m_scene );
	rerenderAndUpdatePreview();

	if( qApp->arguments().size() > 1 && QFileInfo( qApp->arguments()[1] ).exists() )
	{
		loadSettingsFile( qApp->arguments()[1] );
	}
}




MainWindow::~MainWindow()
{
	delete ui;
}



void MainWindow::resizeEvent( QResizeEvent * _ev )
{
	QMainWindow::resizeEvent( _ev );
	updateAutoZoom();
}




QImage MainWindow::renderPage( bool no_margin )
{
	const int pageWidth_px	= convertToPx( ui->pageWidth->value() );
	const int pageHeight_px	= convertToPx( ui->pageHeight->value() );
	const int marginLeft_px	= no_margin ? 0 : convertToPx( ui->marginLeft->value() );
	const int marginTop_px	= no_margin ? 0 : convertToPx( ui->marginTop->value() );
	const int itemWidth_px	= convertToPx( ui->itemWidth->value() );
	const int itemHeight_px	= convertToPx( ui->itemHeight->value() );
	const int barCodeSize = qMin( itemWidth_px, itemHeight_px ) * ui->barcodeScale->value();

	QString barcodeText = ui->barcodeText->text();
	QString barcodeLabel = ui->barcodeLabel->text();
	int seriennummer = ui->serialStart->value();
	const int seriennummer_anz = ui->serialDuplicate->value();
	const int fontSize = ui->labelFontSize->value() * ui->dpi->value() / 300;

	int x = marginLeft_px;
	int y = marginTop_px;

	// Initialisierung
	DmtxEncode * enc = dmtxEncodeCreate();
	if(enc == NULL)
	{
		qFatal( "Could not create dmtx encoder!" );
		return QImage();
	}

	dmtxEncodeSetProp( enc, DmtxPropPixelPacking, DmtxPack32bppRGBX );
	dmtxEncodeSetProp( enc, DmtxPropWidth, itemWidth_px );
	dmtxEncodeSetProp( enc, DmtxPropHeight, itemHeight_px );

	QImage img( pageWidth_px, pageHeight_px, QImage::Format_RGB32 );
	img.fill( QColor( Qt::white ).rgb() );

	QPainter p( &img );
	QFont f = p.font();
	f.setPointSize( fontSize );
	p.setFont( f );

	int i = 1;
	// Endlosschleife bis Blatt voll ist
	while( true )
	{
		QString txt = QString().sprintf( barcodeText.toUtf8().constData(), seriennummer );
		QString label = QString().sprintf( barcodeLabel.toUtf8().constData(), seriennummer );

		// string codieren
		char * txtStr = qstrdup( txt.toAscii().constData() );
		dmtxEncodeDataMatrix( enc, txt.size(), (unsigned char *) txtStr );
		delete[] txtStr;

		QImage curBarCode( enc->image->pxl, enc->image->width, enc->image->height,
									QImage::Format_RGB32 );

		p.drawImage( QRect( x + (itemWidth_px-barCodeSize) / 2, y,
							barCodeSize, barCodeSize ), curBarCode );
		p.drawText( x + (itemWidth_px-p.fontMetrics().width( label ) ) / 2,
					y+ itemHeight_px - p.fontMetrics().height() - 4, label );

		x += itemWidth_px;
		if (x+itemWidth_px > pageWidth_px)
		{
			x = marginLeft_px;
			y += itemHeight_px;
		}
		if (y + itemHeight_px > pageHeight_px)
		{
			break;
		}
		i++;
		if (i > seriennummer_anz)
		{
			seriennummer++;
			i = 1;
		}
	}

	p.end();

	return img;
}




void MainWindow::openSettings( void )
{
	QFileDialog f( this, tr( "Open settings file" ), QString(), "*.qdm" );
	f.setFileMode( QFileDialog::ExistingFile );
	f.setAcceptMode( QFileDialog::AcceptOpen );
	if( f.exec() && !f.selectedFiles().isEmpty() )
	{
		loadSettingsFile( f.selectedFiles().first() );
	}
}




void MainWindow::saveSettings( void )
{
	QFileDialog f( this, tr( "Save settings file" ), QString(), "*.qdm" );
	f.setFileMode( QFileDialog::AnyFile );
	f.setAcceptMode( QFileDialog::AcceptSave );
	if( f.exec() && !f.selectedFiles().isEmpty() )
	{
		QSettings s( f.selectedFiles().first(), QSettings::IniFormat );
		s.setValue( "barcode/text", ui->barcodeText->text() );
		s.setValue( "barcode/label", ui->barcodeLabel->text() );
		s.sync();
	}
}




void MainWindow::rerenderAndUpdatePreview()
{
	m_cachedPreview = renderPage();
	updateAutoZoom();
}




void MainWindow::updatePreview()
{
	QImage img = m_cachedPreview;
	img = img.scaled( img.width() * ui->zoomSlider->value() / 100,
				img.height()* ui->zoomSlider->value() / 100 );
	//m_previewLabel->setFixedSize( img.size() );
	m_previewItem->setPixmap( QPixmap::fromImage( img ) );
	m_scene->setSceneRect( img.rect() );
}




void MainWindow::save()
{
	QFileDialog f( this, tr( "Save file" ), QString(), "*." + ui->outputFormat->currentText().toLower() );
	f.setFileMode( QFileDialog::AnyFile );
	f.setAcceptMode( QFileDialog::AcceptSave );
	if( f.exec() && !f.selectedFiles().isEmpty() )
	{
		if( ui->outputFormat->currentText() == "PDF" )
		{
			QPrinter printer( QPrinter::HighResolution );
			printer.setOutputFileName( f.selectedFiles().first() );
			print( &printer );
		}
		else
		{
			QImage img = renderPage();
			img.save( f.selectedFiles().first() );
		}
	}
}



void MainWindow::print()
{
	QPrintDialog p( this );
	if( p.exec() )
	{
		print( p.printer() );
	}
}




void MainWindow::updateAutoZoom()
{
	if( ui->autoZoom->isChecked() )
	{
		const int sw = ( ui->previewArea->width() - 2 ) * 100 / m_cachedPreview.width();
		const int sh = ( ui->previewArea->height() - 2 ) * 100 / m_cachedPreview.height();
		ui->zoomSlider->setValue( qMin( sw, sh ) );
		ui->autoZoom->setChecked( true );
	}
	updatePreview();
}




void MainWindow::updateZoom()
{
	ui->autoZoom->setChecked( false );
	updatePreview();
}




void MainWindow::loadSettingsFile( const QString & _file )
{
	QSettings s( _file, QSettings::IniFormat );
	ui->barcodeText->setText( s.value( "barcode/text" ).toString() );
	ui->barcodeLabel->setText( s.value( "barcode/label" ).toString() );
}




int MainWindow::convertToPx( int _v )
{
	float factor = 10;
	if( ui->unit->currentText() == "mm" )
	{
		factor = 25.4;
	}
	return (int) ( _v / 25.4 * ui->dpi->value() );
}




void MainWindow::print( QPrinter * _target )
{
	_target->setResolution( ui->dpi->value() );
	QImage img = renderPage( true );
	QPainter painter( _target );
	const int offX = qMax( 0, convertToPx( ui->marginLeft->value() ) -
				( _target->paperRect().width() - _target->pageRect().width() ) / 2 );
	const int offY = qMax( 0, convertToPx( ui->marginTop->value() ) -
				( _target->paperRect().height() - _target->pageRect().height() ) / 2 );
	painter.drawImage( offX, offY, img );
	painter.end();
}




void MainWindow::about( void )
{
	AboutDialog( this ).exec();
}


