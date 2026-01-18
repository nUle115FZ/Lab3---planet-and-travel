#ifndef ADDEDGEDIALOG_H
#define ADDEDGEDIALOG_H

#include <QDialog>
#include "StarGraph.h"

namespace Ui {
class AddEdgeDialog;
}

class AddEdgeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEdgeDialog(StarGraph* graph, QWidget *parent = nullptr);
    ~AddEdgeDialog();
    
    QString getFromPlanet() const;
    QString getToPlanet() const;
    double getDistance() const;
    double getRisk() const;

private:
    Ui::AddEdgeDialog *ui;
};

#endif //aDDEDGEDIALOG_H

