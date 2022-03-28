#include "Labjack.h"
#include "LJM_Utilities.h"

#include <QElapsedTimer>

#ifdef DUMMY_DATA
#include <QRandomGenerator>
#endif

Labjack::Labjack(RunManager * runManager, const QString &config)
    : Component(runManager, config)
{
    load_config(config);
    assert(parse_config({"name" , "channel", "con", "id"}));

    this->elementName = get_value("name");
    channel = get_value("channel").toUInt();
    connectionType = get_value("con").toInt();
    identifier = get_value("id");
}

Labjack::Labjack(RunManager * runManager,
                 const QString &m_element_name,
                 const QString &channel_name,
                 int connectionType,
                 const QString &identifier,
                 const QString &pchannel,
                 const QString &nchannel)
    : Component(m_element_name, runManager),
      connectionType(connectionType),
      identifier(identifier)
{
    this->elementName = m_element_name;

    get_channel_names(channel_name, m_name);
    get_channel_addresses(pchannel, m_pchannel);
    get_channel_addresses(nchannel, m_nchannel);

    assert(m_pchannel.size() == m_nchannel.size());
    assert(m_name.size() == m_pchannel.size());

    channel = m_name.size();
}

Labjack::~Labjack() {
    LJM_Close(handle);

    for(QVector<LabjackChannel*>::iterator it = channel_vec.begin(); it != channel_vec.end(); it++)
    {
        delete (*it);
    }

    delete sampleTimer;
    delete rangeTimer;
    delete dataTimer;

    delete[] aAddresses;
    delete[] aTypes;
    delete[] aValues;
}

void Labjack::set_config()
{
    config_entry_list.clear();

    set_value("name", elementName);
    set_value("channel", QString::number(channel_vec.size()));
    set_value("con", QString::number(connectionType));
    set_value("id", identifier);

    for(int i = 0; i < channel_vec.size(); i++) {
        set_value("c" + QString::number(i) + "n", channel_vec[i]->get_name());
        set_value("c" + QString::number(i) + "pc", QString::number(channel_vec[i]->get_pchan()));
        set_value("c" + QString::number(i) + "nc", QString::number(channel_vec[i]->get_nchan()));
        set_value("c" + QString::number(i) + "b", QString::number(channel_vec[i]->get_boundary()));
        set_value("c" + QString::number(i) + "g", QString::number(channel_vec[i]->get_gain()));
        set_value("c" + QString::number(i) + "r", QString::number(channel_vec[i]->get_range()));
    }
}

void Labjack::open_labjack() {
#ifndef DUMMY_DATA
    is_connected = LJM_Open(LJM_dtANY, connectionType, identifier.toLocal8Bit(), &handle) == 0 ? true : false;
#endif
#ifdef DUMMY_DATA
    is_connected = LJM_Open(LJM_dtANY, LJM_ctANY, LJM_DEMO_MODE, &handle) == 0 ? true : false;
#endif
}

void Labjack::init()
{
    open_labjack();

    /* Setup channel */
    for(uint i = 0; i < channel; i++)
    {
        if(is_empty())
        {
            channel_vec.push_back(new LabjackChannel(m_name[i], &handle, m_pchannel[i], m_nchannel[i]));
        }
        else {
            QString name = get_value("c" + QString::number(i) + "n");

            int p_chan = get_value("c" + QString::number(i) + "pc").toInt();
            int n_chan = get_value("c" + QString::number(i) + "nc").toInt();

            double boundary = get_value("c" + QString::number(i) + "b").toDouble();
            int gain = get_value("c" + QString::number(i) + "g").toInt();
            double range = get_value("c" + QString::number(i) + "r").toDouble();

            channel_vec.push_back(new LabjackChannel(name, &handle, p_chan, n_chan, boundary, range, gain));
        }
    }

    /* Setup labjack access channel */
    foreach (LabjackChannel* channel, channel_vec)
    {
        address_vec.push_back(channel->get_pchan_address());
        type_vec.push_back(LJM_FLOAT32);
        value_vec.push_back(0.0);
    }

    /* Setup labjack readout addresses */
    aAddresses = new int[address_vec.size()];
    aTypes = new int[address_vec.size()];
    aValues = new double[address_vec.size()];

    for(int i = 0; i < address_vec.size(); i++)
    {
        aAddresses[i] = address_vec[i];
        aTypes[i] = type_vec[i];
        aValues[i] = 0.0;
    }

    /* Setup sample timer */
    sampleTimer = new QElapsedTimer;
    rangeTimer = new QElapsedTimer;
    dataTimer = new QElapsedTimer;
    rangeTimer->start();

    update();

    emit init_done();
}

void Labjack::update() {
    sampleTimer->restart();

    /* Gather data */
    read(value_vec);

    QVector<double>::iterator it = value_vec.begin();
    foreach (LabjackChannel* channel, channel_vec)
    {
        channel->update_value(*it++);
    }

    //Log data
    if(logging)
    {
        runManager->append_values_to_file(this, value_vec);
    }

    /* Check and adapt */
    adapt_sample_rate(sampleTimer->nsecsElapsed());

    /* Distribute data */
    dataTimer->restart();
    emit data_available();

    if(dataTimer->elapsed() > 1000/samplerate)
        adapt_sample_rate(dataTimer->nsecsElapsed());
}

void Labjack::set_samplerate(const QString& text)
{
    samplerate = text.toInt() > maxSamplerate ? maxSamplerate : text.toInt();
    samplerate = samplerate > 0 ? samplerate : 1;
    fixedSamplerate = samplerate;

    change_samplerate();
}

int Labjack::read(QVector<double>& value)
{
    int res = 0;

    if(is_connected == false)
    {
        res = 0;
    }
    else
    {
#ifndef DUMMY_DATA
        int errorAddress = INITIAL_ERR_ADDRESS;

        res = LJM_eReadAddresses(handle, address_vec.size(), aAddresses, aTypes, aValues, &errorAddress);
        ErrorCheckWithAddress(res, errorAddress, "LJM_eReadAddresses");
#endif

#ifdef DUMMY_DATA
        create_dummy_data(address_vec.size(), aValues);
#endif

        for(int i = 0; i < address_vec.size(); i++)
        {
            value[i] = aValues[i];
        }
    }

    return res;
}

/* Reconstruct individual channels from user input */
void Labjack::get_channel_addresses(const QString& input, QVector<int>& output)
{
    int position = 0;

    do {
        QString number = input.mid(position,  input.indexOf(',', position));
        output.push_back(number.toInt());
    } while((position = input.indexOf(',', position) + 1) > 0);
}

void Labjack::get_channel_names(const QString& input, QVector<QString>& output)
{
    int position = 0;

     do {
        QString name = input.mid(position,  input.indexOf(',', position));
        name.remove(' ');
        output.push_back(name);
    } while((position = input.indexOf(',', position) + 1) > 0);
}


void Labjack::adapt_channel_range()
{
    if(rangeTimer->elapsed() < 1000)
        return;

    foreach (LabjackChannel *channel, channel_vec)
        channel->set_range();

    rangeTimer->restart();
}

#ifdef DUMMY_DATA
void Labjack::create_dummy_data(int size, double *values)
{
    for(int i = 0; i < size; i++)
    {
        values[i] = channel_vec[i]->get_value()
                + double(QRandomGenerator::global()->bounded(-qint16(4096), qint16(4096))) / double(1024);
    }
}
#endif

void Labjack::adapt_sample_rate(qint64 nsecs)
{
    /* Convert to sample per second */
    if(nsecs < 1000*1000)
    {
        maxSamplerate = 1000;
    }
    else
    {
        maxSamplerate = 1000*1000*1000 / (2*nsecs);
    }

    if((maxSamplerate < samplerate) || (is_maximum && samplerate != maxSamplerate)) {
        samplerate = maxSamplerate;
        change_samplerate();
    }

    if(!is_maximum && samplerate != fixedSamplerate) {
        samplerate = maxSamplerate < fixedSamplerate ? maxSamplerate : fixedSamplerate;
        change_samplerate();
    }
}

QStringList Labjack::generate_header()
{
    QStringList header;

    foreach (LabjackChannel * channel, channel_vec)
        header.push_back(channel->get_name());

    return header;
}
