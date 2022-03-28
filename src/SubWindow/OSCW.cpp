#include "OSCW.h"

#include "src/Components/OSC.h"
#include "src/Plot/OSCPlot.h"

OSCW::OSCW(RunManager *m_runManager, OSC *osc)
    : SubWindow(m_runManager, osc), osc(osc) {

    /* Connect and register signals */
    //connect start
    //connect stop
}

OSCW::~OSCW() {
    /* Degregister signals */

    delete osc;
}

void OSCW::create_layout() {
    QVBoxLayout *mainVLayout = new QVBoxLayout;

    /* Running Mode */
    QHBoxLayout *modeLayout = new QHBoxLayout;
    QPushButton *modeRunButton = new QPushButton("Start / Stop");
//    connect(modeRunButton, SIGNAL(clicked()), this, osc, SLOT())
    modeLayout->addWidget(modeRunButton);

    /* Trigger Mode */
    QGroupBox *triggerBox = new QGroupBox("Trigger:");
    QHBoxLayout *triggerButtonLayout = new QHBoxLayout;

    QRadioButton *triggerAuto = new QRadioButton("Auto");
    triggerButtonLayout->addWidget(triggerAuto);
    //connect(triggerAuto)

    QRadioButton *triggerNormal = new QRadioButton("Normal");
    triggerButtonLayout->addWidget(triggerNormal);
    //connect(triggerAuto)

    QRadioButton *triggerSingle = new QRadioButton("Single");
    triggerButtonLayout->addWidget(triggerSingle);
    //connect(triggerAuto)

    triggerBox->setLayout(triggerButtonLayout);

    modeLayout->addWidget(triggerBox);

    //Trigger: Auto, Normal, Single (Radio Button)
    //Trigger: Channel (Dropdown)

    //Timebase (Dropdown)

    /* channels */
        //Set: Enabled (Checkbox)

        //Get: VMax, VMin, Vpp, Hz (Disabled QLineEdit)

        //Set: Range, Offset (QLineEdit)

    /* Plot */
    //plot->add(OSCChannel);
    //connect(osc->range, plot->range);

    mainVLayout->addLayout(modeLayout);

    setLayout(mainVLayout);

    emit layout_done();
}
