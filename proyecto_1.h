#ifndef PROYECTO_1_H
#define PROYECTO_1_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class proyecto_1; }
QT_END_NAMESPACE

class proyecto_1 : public QMainWindow
{
    Q_OBJECT

public:
    proyecto_1(QWidget *parent = nullptr);
    ~proyecto_1();

private slots:
    void on_inicioBoton_clicked();

private:
    Ui::proyecto_1 *ui;

};
#endif // PROYECTO_1_H
