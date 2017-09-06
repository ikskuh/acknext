#ifndef ACKCOLORSELECTOR_HPP
#define ACKCOLORSELECTOR_HPP

#include <QWidget>

namespace Ui {
	class AckColorSelector;
}

class AckColorSelector : public QWidget
{
	Q_OBJECT

public:
	explicit AckColorSelector(QWidget *parent = 0);
	~AckColorSelector();

private:
	Ui::AckColorSelector *ui;
};

#endif // ACKCOLORSELECTOR_HPP
