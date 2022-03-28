#ifndef INDICATORBOX_H
#define INDICATORBOX_H

/*
 * Author: Mattis Jaksch
 *
 * Checkbox together with an icon to indicate the status
 *
*/

#include <QCheckBox>

class IndicatorBox : public QCheckBox
{
Q_OBJECT

public:
    IndicatorBox(QIcon onIcon, QIcon offIcon, QIcon eventIcon);
    virtual ~IndicatorBox();

public slots:
    void set_individual_icon(int);
    void set_event_icon(bool);
    void set_disabled(bool);

private:
    QIcon* lastIcon;
    QIcon* onIcon;
    QIcon* offIcon;
    QIcon* eventIcon;
};

#endif
