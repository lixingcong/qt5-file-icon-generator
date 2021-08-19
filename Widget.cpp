/*
 * Widge.cpp
 *
 * FIXME: 功能简要概述
 *
 * Created on: 2021年 8月 16日
 * Author: lixingcong
 */

#include "Widget.h"
#include <QPainter>
#include <QPainterPath>
#include <QSizeF>
#include <QPointF>
#include <QString>
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QBoxLayout>
#include <QGraphicsRectItem>
#include <QLineEdit>
#include <QLabel>
#include <QColorDialog>
#include <QPushButton>
#include <QFontDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QSlider>
#include <QFileDialog>
#include <QFile>

struct FileIconGenerator
{
	FileIconGenerator()
	    : canvasWidth(64)
	    , fileRect(QRectF(8, 1, 48, canvasWidth - 2))
	    , fileRoundCornerRadius(4)
	    , fileFoldWidth(16)
	    , typeRect(QRectF(4, 25, 56, 24))
	    , ratio(1)
	{}

	qreal canvasWidth; // 画布大小

	QRectF fileRect; // 文件框
	qreal  fileRoundCornerRadius; // 文件框圆角半径
	qreal  fileFoldWidth; // 文件框右上角褶皱的宽（与高相等）

	QRectF  typeRect; // 文件类型框
	qreal   ratio;

	FileIconGenerator& operator*=(const qreal& rhs)
	{
		canvasWidth *= rhs;
		ratio *= rhs;

		const auto fileRectTopLeft = fileRect.topLeft() * rhs;
		const auto fileRectSize    = fileRect.size() * rhs;
		fileRect                   = QRectF(fileRectTopLeft, fileRectSize);

		fileRoundCornerRadius *= rhs;
		fileFoldWidth *= rhs;

		const auto typeRectTopLeft = typeRect.topLeft() * rhs;
		const auto typeRectSize    = typeRect.size() * rhs;
		typeRect                   = QRectF(typeRectTopLeft, typeRectSize);

		return *this;
	}

	QPixmap toPixmap(const QString& text, const QColor& textColor, const QColor& backgroundColor, const QFont& font) const
	{
		const QRectF rect(0, 0, canvasWidth, canvasWidth);
		const QPointF fileRectTopRight(fileRect.topRight());

		QPixmap pixmap(rect.size().toSize());
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing);

		QBrush       brush(backgroundColor); //(QColor(226, 229, 231));
		QPainterPath path;
		// ----------- file base ----------- 1
		path.addRoundedRect(fileRect, fileRoundCornerRadius, fileRoundCornerRadius);
		{
			QPainterPath uselessPath;
			uselessPath.addPolygon(QPolygonF() << fileRectTopRight << fileRectTopRight + QPointF(-fileFoldWidth, 0)
			                                   << fileRectTopRight + QPointF(0, fileFoldWidth));
			uselessPath.closeSubpath();
			path -= uselessPath;
		}
		painter.fillPath(path, brush);
		// ----------- file base ----------- 2

		// ----------- file folding ----------- 1
		path = QPainterPath();
		path.moveTo(fileRectTopRight + QPointF(-fileFoldWidth, 0));
		path.lineTo(fileRectTopRight + QPointF(-fileFoldWidth, fileFoldWidth - fileRoundCornerRadius));
		path.arcTo(QRectF(path.currentPosition() + QPointF(0, -fileRoundCornerRadius), QSizeF(2 * fileRoundCornerRadius, 2 * fileRoundCornerRadius)), 180, 90);
		path.lineTo(fileRectTopRight + QPointF(0, fileFoldWidth));
		path.closeSubpath();
		brush.setColor(backgroundColor.darker(150));
		painter.fillPath(path, brush);
		// ----------- file folding ----------- 2

		// ----------- file format ----------- 1
		if (!text.isEmpty())
		{
			brush.setColor(textColor);
			painter.setBrush(brush);
			painter.setPen(QPen(brush.color()));
			QFont f(font);
			f.setPointSizeF(f.pointSizeF() * ratio);
			painter.setFont(f);
			painter.drawText(typeRect, Qt::AlignCenter, text);
		}
		// ----------- file format ----------- 2

		return pixmap;
	}
};

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , m_cncZero(new FileIconGenerator)
    , m_pixmapItem(new QGraphicsPixmapItem)
    , m_rectItem(new QGraphicsRectItem)
    , m_labelSize(new QLabel)
    , m_textColor(Qt::white)
    , m_backgroundColor(QColor(232, 24, 37))
    , m_text("ABC")
    , m_font(QFont("Arial", 12))
{
	m_font.setBold(true);
	setMinimumSize(400, 300);
	setWindowTitle("File Icon Generator");

	auto mainLayout = new QHBoxLayout(this);
	auto view       = new QGraphicsView(this);
	auto scene      = new QGraphicsScene(this);
	view->setScene(scene);
	scene->addItem(m_pixmapItem);
	scene->addItem(m_rectItem);
	mainLayout->addWidget(view);
	QPen rectItemPen(Qt::gray);
	rectItemPen.setStyle(Qt::DashLine);
	m_rectItem->setPen(rectItemPen);

	// ----------- params ----------- 1
	auto groupBox   = new QGroupBox("Params", this);
	auto formLayout = new QFormLayout(groupBox);
	mainLayout->addWidget(groupBox);
	groupBox->setFixedWidth(200);

	formLayout->addRow(new QLabel("Size"), m_labelSize);

	auto ratioSlider = new QSlider(Qt::Horizontal);
	ratioSlider->setRange(1, 10);
	ratioSlider->setValue(m_cncZero->ratio);
	formLayout->addRow(new QLabel("Ratio"), ratioSlider);
	connect(ratioSlider, SIGNAL(valueChanged(int)), this, SLOT(setRatio(int)));

	auto lineEdit = new QLineEdit(m_text);
	formLayout->addRow(new QLabel("Text"), lineEdit);
	connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(_setText(QString)));

	auto textColorBtn = new QPushButton("Pick one");
	auto bgColorBtn   = new QPushButton("Pick one");
	connect(textColorBtn, SIGNAL(clicked(bool)), this, SLOT(onClickedTextColorBtn()));
	connect(bgColorBtn, SIGNAL(clicked(bool)), this, SLOT(onClickedBgColorBtn()));
	formLayout->addRow(new QLabel("Text Color"), textColorBtn);
	formLayout->addRow(new QLabel("Background Color"), bgColorBtn);

	auto fontBtn = new QPushButton("Change");
	connect(fontBtn, SIGNAL(clicked(bool)), this, SLOT(onClickedFontBtn()));
	formLayout->addRow(new QLabel("Font"), fontBtn);

	auto saveBtn = new QPushButton("To File...");
	formLayout->addRow(new QLabel("Save"), saveBtn);
	connect(saveBtn, SIGNAL(clicked(bool)), this, SLOT(onClickedSaveBtn()));
	// ----------- params ----------- 2

	rebuild();
}

Widget::~Widget()
{
	delete m_cncZero;
}

void Widget::setRatio(int r)
{
	qreal      newRatio = (qreal) r / m_cncZero->ratio;
	m_cncZero->operator*=(newRatio);
	rebuild();
}

void Widget::onClickedTextColorBtn()
{
	auto dialog = new QColorDialog(m_textColor, this);
	dialog->setGeometry(QRect(mapToGlobal(rect().topRight()), dialog->size()));
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	connect(dialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(_setTextColor(QColor)));
	dialog->open();
}

void Widget::onClickedBgColorBtn()
{
	auto dialog = new QColorDialog(m_backgroundColor, this);
	dialog->setGeometry(QRect(mapToGlobal(rect().topRight()), dialog->size()));
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	connect(dialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(_setBackgroundColor(QColor)));
	dialog->open();
}

void Widget::onClickedFontBtn()
{
	auto dialog = new QFontDialog(m_font, this);
	dialog->setGeometry(QRect(mapToGlobal(rect().topRight()), dialog->size()));
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	connect(dialog, SIGNAL(currentFontChanged(QFont)), this, SLOT(_setFont(QFont)));
	dialog->open();
}

void Widget::onClickedSaveBtn()
{
	const QString fileName = QFileDialog::getSaveFileName(this, "Save", "1.png", "PNG Files (*.png);;");
	if(!fileName.isEmpty()){
		QFile file(fileName);
		if(file.open(QIODevice::WriteOnly|QIODevice::Truncate))
			m_pixmap.save(&file, "PNG");
	}
}

void Widget::rebuild()
{
	m_pixmap = m_cncZero->toPixmap(m_text, m_textColor, m_backgroundColor, m_font);
	m_pixmapItem->setPixmap(m_pixmap);
	m_rectItem->setRect(QRectF(QPointF(-0.5, -0.5), m_pixmap.size() + QSizeF(0.5, 0.5)));
	m_labelSize->setText(QString("%1x%2").arg(m_pixmap.width()).arg(m_pixmap.height()));
}
