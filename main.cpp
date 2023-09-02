#pragma once
#pragma execution_character_set("utf-8")

#include "stdafx.h"

#include "myCalc.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	myCalc w;
	w.show();

	return QApplication::exec();
}
