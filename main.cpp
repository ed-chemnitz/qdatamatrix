//
// Erstellen von 2D-Datamatrix-Barcodes mit libdmtx und Qt
// Author:  Niko Scheibe, Tobias Doerffel
// Datum:   02.04.2009
// Version: 0.2
//

#include <QtGui/QApplication>

#include "mainwindow.h"

int main( int argc, char *argv[] )
{
	QApplication app( argc, argv );

	MainWindow m;
	m.show();

	return app.exec();
}

