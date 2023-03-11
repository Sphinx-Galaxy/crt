#include "LBJW.h"

#include "src/Components/Labjack.h"
#include "src/Plot/LBJPlot.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QVBoxLayout>

LBJW::LBJW(RunManager* m_runManager, Labjack* lbj)
    : SubWindow(m_runManager, lbj), lbj(lbj)
{
}

LBJW::~LBJW()
{
    delete lbj;
}

void LBJW::create_layout()
{
    QVBoxLayout* mainVLayout = new QVBoxLayout;

    QHBoxLayout* topLineLayout = new QHBoxLayout;
    QGroupBox* settingsBox = new QGroupBox("Settings");

    /* Precision slider from 1 to 10 */
    QSlider* precisionSlide = new QSlider(Qt::Horizontal);
    precisionSlide->setRange(1, 10);
    precisionSlide->setTickPosition(QSlider::TicksBelow);
    precisionSlide->setTickInterval(1);
    precisionSlide->setValue(1);
    connect(precisionSlide, SIGNAL(valueChanged(int)), lbj, SLOT(set_main_resolution(int)));
    topLineLayout->addWidget(new QLabel("Resolution"));
    topLineLayout->addWidget(precisionSlide);

    /* Measurement settling time */
    QLineEdit* settlingLine = new QLineEdit;
    QIntValidator* settlingValid = new QIntValidator(1, 50000, this);
    settlingLine->setText(QString::number(10000));
    settlingLine->setValidator(settlingValid);
    connect(settlingLine, SIGNAL(textChanged(const QString &)), lbj, SLOT(set_main_settling(const QString &)));
    topLineLayout->addWidget(new QLabel("Settling [us]:"));
    topLineLayout->addWidget(settlingLine);

    /* Measurement datarate */
    QLineEdit* datarateLine = new QLineEdit;
    QIntValidator* datarateValid = new QIntValidator(1, 1000, this);
    datarateLine->setText(QString::number(1));
    datarateLine->setValidator(datarateValid);
    connect(datarateLine, SIGNAL(textChanged(const QString &)), lbj, SLOT(set_samplerate(const QString &)));
    topLineLayout->addWidget(new QLabel("Datarate [S/s]:"));
    topLineLayout->addWidget(datarateLine);

    QLabel* currentDatarateLine = new QLabel;
    currentDatarateLine->setText(QString::number(1));
    currentDatarateLine->setMinimumWidth(40);
    connect(lbj, SIGNAL(samplerate_changed(const QString &)), currentDatarateLine, SLOT(setText(const QString &)));
    topLineLayout->addWidget(new QLabel("/"));
    topLineLayout->addWidget(currentDatarateLine);

    /* Maximum button */
    QCheckBox* datarateBox = new QCheckBox("Maximum");
    connect(datarateBox, SIGNAL(stateChanged(int)), lbj, SLOT(set_maximum_samplerate(int)));
    topLineLayout->addWidget(datarateBox);

    /* Permanent Logging */
    QCheckBox* permanentLogBox = new QCheckBox("Permanent Logging");
    permanentLogBox->setDisabled(!runManager->is_valid());
    connect(permanentLogBox, SIGNAL(stateChanged(int)), lbj, SLOT(set_permanent_logging(int)));
    connect(runManager, SIGNAL(isInvalid_changed(bool)), permanentLogBox, SLOT(setDisabled(bool)));
    topLineLayout->addWidget(permanentLogBox);

    /* Signal button */
    QPushButton* signalButton = new QPushButton;
    signalButton->setText("Add Signal");
    connect(signalButton, SIGNAL(clicked()), this, SLOT(show_signal_dialog()));
    topLineLayout->addWidget(signalButton);

    settingsBox->setLayout(topLineLayout);

    /* Plot for the channels */
    QVBoxLayout* graphLayout = new QVBoxLayout;
    QGroupBox* graphBox = new QGroupBox("Plot");
    QCustomPlot *plot = new QCustomPlot(this);
    plot->setGeometry(QRect());
    plot->setMinimumHeight(256);
    LBJPlot* lbjplot = new LBJPlot(plot);
    graphLayout->addWidget(plot);
    graphBox->setLayout(graphLayout);
    connect(datarateLine, SIGNAL(textChanged(const QString &)), lbjplot, SLOT(set_datarate(const QString &)));
    connect(lbj, SIGNAL(data_available()), lbjplot, SLOT(update_plot()));

    /* Channel Layout */
    QGridLayout* channelLayout = new QGridLayout;
    QGroupBox* channelBox = new QGroupBox("Channel");

    /* Channel header */
    channelLayout->addWidget(new QLabel("Name"), 0, 0);
    channelLayout->addWidget(new QLabel("Boundary"), 0, 1);
    channelLayout->addWidget(new QLabel("Value"), 0, 2);
    channelLayout->addWidget(new QLabel("Virtual Gain"), 0, 3);
    channelLayout->addWidget(new QLabel("Range"), 0, 4);
    channelLayout->addWidget(new QLabel("Graph"), 0, 5);

    QTimer* timer = new QTimer;

    int cnt = 1;
    foreach (LabjackChannel *channel, lbj->get_channel_vec()) {
        //Name
        QLabel* channelLabel = new QLabel(channel->get_name());

        //Boundary
        QLineEdit *boundaryLine = new QLineEdit(QString::number(channel->get_boundary()));
        connect(boundaryLine, SIGNAL(textChanged(const QString &)), channel, SLOT(set_boundary(const QString &)));

        /* Value */
        QLabel *valueLine = new QLabel(QString::number(channel->get_value()));
        valueLine->setMinimumWidth(100);
        connect(timer, SIGNAL(timeout()), channel, SLOT(refresh_value()));
        connect(channel, SIGNAL(value_refreshed(const QString &)), valueLine, SLOT(setText(const QString &)));

        //Gain
        QLineEdit* gainLine = new QLineEdit(QString::number(channel->get_gain()));
        connect(gainLine, SIGNAL(textChanged(const QString &)), channel, SLOT(set_virtual_gain(const QString &)));

        /* Range */
        QHBoxLayout* rangeLayout = new QHBoxLayout;
        QSlider* rangeSlide = new QSlider(Qt::Horizontal);
        rangeSlide->setRange(0, 3);
        rangeSlide->setTickPosition(QSlider::TicksBelow);
        rangeSlide->setTickInterval(1);
        rangeSlide->setValue(channel->get_range_index());
        connect(rangeSlide, SIGNAL(valueChanged(int)), channel, SLOT(set_range(int)));

        QLabel* rangeLine = new QLabel(QString::number(channel->get_gain()));
        rangeLine->setText(QString::number(channel->get_range()));
        rangeLine->setMinimumWidth(20);
        connect(channel, SIGNAL(range_changed(const QString&)), rangeLine, SLOT(setText(const QString &)));

        rangeLayout->addWidget(rangeSlide);
        rangeLayout->addWidget(rangeLine);

        /* Graph */
        QCheckBox* graphCheckBox = new QCheckBox;
        graphCheckBox->setStyleSheet("color: " + color_list[cnt%color_list.size()].name + " ;");
        lbjplot->add_channel(channel, color_list[cnt%color_list.size()].color);
        connect(graphCheckBox, SIGNAL(stateChanged(int)), lbjplot->get_plotelement_list().last(), SLOT(set_plot_active(int)));

        channelLayout->addWidget(channelLabel, cnt, 0);
        channelLayout->addWidget(boundaryLine, cnt, 1);
        channelLayout->addWidget(valueLine, cnt, 2);
        channelLayout->addWidget(gainLine, cnt, 3);
        channelLayout->addLayout(rangeLayout, cnt, 4);
        channelLayout->addWidget(graphCheckBox, cnt, 6);
        cnt++;

        connect(channel, SIGNAL(boundary_check_failed()), this, SLOT(trigger_signal_list()));
    }

    channelBox->setLayout(channelLayout);

    mainVLayout->addWidget(settingsBox);
    mainVLayout->addWidget(channelBox);
    mainVLayout->addWidget(graphBox);

    timer->start(500);

    setLayout(mainVLayout);

    emit layout_done();
}
