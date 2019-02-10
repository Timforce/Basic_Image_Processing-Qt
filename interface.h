#ifndef INTERFACE_H
#define INTERFACE_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QBuffer>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>

namespace Ui {
class Interface;
}

class Interface : public QMainWindow
{
    Q_OBJECT

public:
    explicit Interface(QWidget *parent = nullptr);
    ~Interface();

private slots:
    void on_actionOpen_triggered();

    void on_imgInv_clicked();

    void on_imgHistEql_clicked();

    void on_imgGammaCor_clicked();

    void on_actionSave_as_triggered();

    void on_imgReset_clicked();

private:
    Ui::Interface *ui;
    QPixmap original_img;
    QPixmap processed_img;
    uchar proc_stat = 0;
    const uchar img_loaded = 1 << 0;
    const uchar hist_done = 1 << 1;
    const uchar gamma_done = 1 << 2;
    const uchar inv_done = 1 << 3;
    bool check_status(void);
    bool check_status(const uchar);
};

#endif // INTERFACE_H
