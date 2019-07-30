#include "eyesdetect.h"
#include <QtWidgets/QApplication>
#include <QTextCodec>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	eyesdetect w;
	w.show();
	return a.exec();
}
