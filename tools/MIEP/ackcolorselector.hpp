#ifndef ACKCOLORSELECTOR_HPP
#define ACKCOLORSELECTOR_HPP

#include <acknext.h>
#include <QWidget>

namespace Ui {
	class AckColorSelector;
}

class AckColorSelector : public QWidget
{
	Q_OBJECT
	COLOR mColor;
public:
	explicit AckColorSelector(QWidget *parent = 0);
	~AckColorSelector();

	COLOR color() const {
		return this->mColor;
	}

	void setColor(COLOR color);

signals:
	void colorChanged(COLOR newColor);

private slots:
	void on_showPicker_clicked();

private:
	Ui::AckColorSelector *ui;
};

#endif // ACKCOLORSELECTOR_HPP
