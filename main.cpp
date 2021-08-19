/*
 * %{Cpp:License:FileName}
 *
 * FIXME: 功能简要概述
 *
 * Created on: 2021年 8月 16日
 * Author: lixingcong
 */

#include "Widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Widget w;
	w.show();
	return a.exec();
}
