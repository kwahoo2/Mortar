#include "CorrectionTable.h"
#include "ui_CorrectionTable.h"
#include <QChart>
#include <QLineSeries>

CorrectionTable::CorrectionTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CorrectionTable)
{
    ui->setupUi(this);

    //Initiate curve with end points
    initVectorValues();
    updateTableChart();

}

CorrectionTable::~CorrectionTable()
{
    delete ui;
}

void CorrectionTable::on_addCorrButton_clicked()
{
    double correctedAlt, correctedAzi;

    correctedAlt = currAlt - baseAlt + currAltCorr;
    correctedAzi = currAzi - baseAzi + currAziCorr;

    emit alignAlt(correctedAlt); //align current and target values to avoid movement when point is added
    emit alignAzi(correctedAzi);

    for (ulong i = 0; i < altCorrVect.size(); ++i)
    {
        if (abs(altCorrVect.at(i).first - currAlt) < pointsMinDist) //Remove old point if it is too near to new one this value
        {
           qDebug() << "Alt point erased";
           altCorrVect.erase(altCorrVect.begin() + static_cast<long>(i));
        }

    }

    for (ulong i = 0; i < aziCorrVect.size(); ++i)
    {
        if (abs(aziCorrVect.at(i).first - currAzi) < pointsMinDist)
        {
            qDebug() <<"Azi point erased";
           aziCorrVect.erase(aziCorrVect.begin() + static_cast<long>(i));
        }

    }

    std::pair<double, double> altPair;
    altPair = std::make_pair (currAlt, correctedAlt);
    std::pair<double, double> aziPair;
    aziPair = std::make_pair (currAzi, correctedAzi);
    altCorrVect.push_back(altPair);
    aziCorrVect.push_back(aziPair);
    qDebug() << "Correction points: " << altPair.first << "," << altPair.second << ";"
             << aziPair.first << ", " << aziPair.second <<"added";
    std::sort(altCorrVect.begin(), altCorrVect.end());
    std::sort(aziCorrVect.begin(), aziCorrVect.end());

    updateTableChart();
}


void CorrectionTable::setCurrAlt (double val)
{
    currAlt = val;
    ui->baseAltLabel->setNum(currAlt);
    calculateCorrAlt(val);
}
void CorrectionTable::setCurrAzi (double val)
{
    currAzi = val;
    ui->baseAziLabel->setNum(currAzi);
    calculateCorrAzi(val);
}
void CorrectionTable::setCurrAltCorr (double val)
{
    currAltCorr = val;
}
void CorrectionTable::setCurrAziCorr (double val)
{
    currAziCorr = val;
}

void CorrectionTable::calculateCorrAlt (double alt)
{
    for (ulong i = 0; i < altCorrVect.size() - 1; ++i) // -1 because operating on sections
    {
        double alt0 = 0.0, alt1 = 0.0, altCorr0 = 0.0, altCorr1 = 0.0;
        if ((alt >= altCorrVect.at(i).first) && (alt < altCorrVect.at(i + 1).first))
        {
            alt0 = altCorrVect.at(i).first;
            alt1 = altCorrVect.at(i + 1).first;
            altCorr0 = altCorrVect.at(i).second;
            altCorr1 = altCorrVect.at(i + 1).second;

            double altCorrected = alt * (altCorr1 - altCorr0) / (alt1 - alt0);
            ui->corrAltLabel->setNum(altCorrected);
            emit sendCorrectedAlt(altCorrected);
        }
    }
}
void CorrectionTable::calculateCorrAzi(double azi)
{
    for (ulong i = 0; i < aziCorrVect.size() - 1; ++i) // -1 because operating on sections
    {
        double azi0 = 0.0, azi1 = 0.0, aziCorr0 = 0.0, aziCorr1 = 0.0;
        if ((azi >= aziCorrVect.at(i).first) && (azi < aziCorrVect.at(i + 1).first))
        {
            azi0 = aziCorrVect.at(i).first;
            azi1 = aziCorrVect.at(i + 1).first;
            aziCorr0 = aziCorrVect.at(i).second;
            aziCorr1 = aziCorrVect.at(i + 1).second;

            double aziCorrected = azi * (aziCorr1 - aziCorr0) / (azi1 - azi0);
            ui->corrAziLabel->setNum(aziCorrected);
            emit sendCorrectedAzi(aziCorrected);

        }
    }

}

void CorrectionTable::on_setBaseButton_clicked()
{
    ui->addCorrButton->setEnabled(true);
    baseAlt = currAltCorr;
    baseAzi = currAziCorr;
    updateTableChart();
}

void CorrectionTable::on_clearButton_clicked()
{
    initVectorValues();
    updateTableChart();
}

void CorrectionTable::updateTableChart()
{
    ui->tableWidget->clearContents(); //Since "The table takes ownership of the item." this should delete items from heap

    ui->tableWidget->setRowCount(static_cast<int>(altCorrVect.size()));

    QLineSeries * seriesAlt = new QLineSeries();
    QLineSeries * seriesAzi = new QLineSeries();
    seriesAlt->setName("Altitude");
    seriesAzi->setName("Azimuth");

    for (ulong i = 0; i < altCorrVect.size(); ++i)
    {
        /*Table part*/
        QTableWidgetItem * itemAlt = new QTableWidgetItem(QString::number(altCorrVect.at(i).first));
        ui->tableWidget->setItem(static_cast<int>(i), 2, itemAlt);

        QTableWidgetItem * itemAltCorr = new QTableWidgetItem(QString::number(altCorrVect.at(i).second));
        ui->tableWidget->setItem(static_cast<int>(i), 3, itemAltCorr);

        /*Graph part*/
        seriesAlt->append(altCorrVect.at(i).first, altCorrVect.at(i).second);

    }

    for (ulong i = 0; i < aziCorrVect.size(); ++i)
    {
        /*Table part*/
        QTableWidgetItem * itemAzi = new QTableWidgetItem(QString::number(aziCorrVect.at(i).first));
        ui->tableWidget->setItem(static_cast<int>(i), 0, itemAzi);

        QTableWidgetItem * itemAziCorr = new QTableWidgetItem(QString::number(aziCorrVect.at(i).second));
        ui->tableWidget->setItem(static_cast<int>(i), 1, itemAziCorr);

        /*Graph part*/
        seriesAzi->append(aziCorrVect.at(i).first, aziCorrVect.at(i).second);

    }
    QChart * chart = new QChart();
    chart->addSeries(seriesAlt);
    chart->addSeries(seriesAzi);
    chart->createDefaultAxes();

    QChart * oldchart = ui->chartView->chart();
    ui->chartView->setChart(chart);

    if (oldchart != nullptr)
        delete oldchart; //To avoid memory leaks, the previous chart must be deleted.
}

void CorrectionTable::initVectorValues()
{
    altCorrVect.clear();
    aziCorrVect.clear();
    altCorrVect.push_back(std::make_pair(-180.0, -180.0));
    altCorrVect.push_back(std::make_pair(180.0, 180.0));
    aziCorrVect.push_back(std::make_pair(0.0, 0.0));
    aziCorrVect.push_back(std::make_pair(360.0, 360.0));
}
