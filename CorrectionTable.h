#ifndef CORRECTIONTABLE_H
#define CORRECTIONTABLE_H

#include <QDialog>

namespace Ui {
class CorrectionTable;
}

class CorrectionTable : public QDialog
{
    Q_OBJECT

public:
    explicit CorrectionTable(QWidget *parent = nullptr);
    ~CorrectionTable();

public slots:
    void setCurrAlt (double val);
    void setCurrAzi (double val);
    void setCurrAltCorr (double val);
    void setCurrAziCorr (double val);

private slots:
    void on_addCorrButton_clicked();

    void on_setBaseButton_clicked();

    void on_clearButton_clicked();

private:
    Ui::CorrectionTable *ui;

    std::vector< std::pair <double, double >> altCorrVect;
    std::vector< std::pair <double, double >> aziCorrVect;

    double baseAlt = 0.0;
    double baseAzi = 0.0;
    double currAlt = 0.0;
    double currAzi = 0.0;
    double currAltCorr = 0.0;
    double currAziCorr = 0.0;

    double pointsMinDist = 2.0;

    void calculateCorrAlt (double alt);
    void calculateCorrAzi (double azi);

    void updateTableChart();
    void initVectorValues();


signals:
    void sendCorrectedAlt (double altCorr);
    void sendCorrectedAzi (double aziCorr);
    void sendManualAltCorr (double alt);
    void sendManualAziCorr (double azi);
};

#endif // CORRECTIONTABLE_H
