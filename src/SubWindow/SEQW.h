#ifndef SEQW_H
#define SEQW_H

/*
 * Author: Mattis Jaksch
 *
 * Holds a sequencer with multiple tasks
 * that get executed during the run
 *
*/

class Sequencer;
class Task;

#include "SubWindow.h"

class SEQW : public SubWindow {
Q_OBJECT

public:
    SEQW(RunManager* m_runManager, Sequencer* sequencer);
    virtual ~SEQW() override;

public slots:
    void set_task(Task*);

private:
    Sequencer* seq;

    void create_layout() override;
};

#endif // SEQW_H
