/*
 * %{Cpp:License:FileName}
 *
 * FIXME: 功能简要概述
 *
 * Created on: 2021年 8月 16日
 * Author: lixingcong
 */

#ifndef WIDGET_H
#define WIDGET_H

#include <QPixmap>
#include <QWidget>

struct FileIconGenerator;
class QGraphicsPixmapItem;
class QGraphicsRectItem;
class QLabel;

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget* parent = nullptr);
	~Widget();

protected Q_SLOTS:
	inline void _setText(const QString& t)
	{
		m_text = t;
		rebuild();
	}

	inline void _setTextColor(const QColor& c)
	{
		m_textColor = c;
		rebuild();
	}

	inline void _setBackgroundColor(const QColor& c)
	{
		m_backgroundColor = c;
		rebuild();
	}

	inline void _setFont(const QFont& f)
	{
		m_font = f;
		rebuild();
	}

	void setRatio(int r);
	void onClickedTextColorBtn();
	void onClickedBgColorBtn();
	void onClickedFontBtn();
	void onClickedSaveBtn();

protected:
	void rebuild();

protected:
	FileIconGenerator* const        m_cncZero;
	QGraphicsPixmapItem* const m_pixmapItem;
	QGraphicsRectItem* const   m_rectItem;
	QLabel* const              m_labelSize;

	// ----------- params ----------- 1
	QColor  m_textColor;
	QColor  m_backgroundColor;
	QString m_text;
	QFont   m_font;
	// ----------- params ----------- 2

	QPixmap m_pixmap;
};
#endif // WIDGET_H
