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
	QColor mColor;
	var mOverdrive;
public:
	explicit AckColorSelector(QWidget *parent = 0);
	~AckColorSelector();

	COLOR color() const {
		return (COLOR) {
			this->mOverdrive * float(this->mColor.redF()),
			this->mOverdrive * float(this->mColor.greenF()),
			this->mOverdrive * float(this->mColor.blueF()),
			1.0
		};
	}

	void setColor(COLOR color);

	QColor getNormalizedColor() const {
		return this->mColor;
	}

	var getOverdrive() const {
		return this->mOverdrive;
	}

signals:
	void colorChanged(COLOR newColor);

private slots:
	void on_showPicker_clicked();

	void on_driver_valueChanged(int value);
private:
	void updateDisplay();

private:
	Ui::AckColorSelector *ui;
};

#endif // ACKCOLORSELECTOR_HPP
