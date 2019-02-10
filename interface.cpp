#include "interface.h"
#include "ui_interface.h"
#include <math.h>

Interface::Interface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Interface)
{
    ui->setupUi(this);
    ui->status->setText("Welcome!");
    this->setFixedSize(QSize(850, 550));
}

Interface::~Interface()
{
    delete ui;
}

void Interface::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Image File", nullptr,
                                                    "Images (*.jpg *.jpeg *.JPG *.png *.PNG *.bmp *.BMP );;"
                                                    "JPG (*.jpg *.jpeg *.JPG);;"
                                                    "PNG (*.png *.PNG);;"
                                                    "BMP (*.bmp *.BMP)");
    if(!original_img.load(filename))
    {
        if(static_cast<bool>(proc_stat & img_loaded))
            return;
        ui->status->setText("Didn't load the image!");
        proc_stat = 0;
        return;
    }
    else {
        processed_img = original_img.copy();
        ui->orig_frame->setPixmap(original_img.scaled(ui->orig_frame->size(), Qt::KeepAspectRatio));
        ui->proc_frame->setPixmap(processed_img.scaled(ui->proc_frame->size(), Qt::KeepAspectRatio));
        ui->status->setText("Loaded image: " + filename);
        proc_stat = 1;
    }
}

void Interface::on_imgInv_clicked()
{
    if (check_status(inv_done))
        return;
    QImage proc = original_img.copy().toImage();
    uchar *bit = proc.bits();
    int h = proc.height();
    int w = proc.width();
    int i, j, dest_index;
    for(i = 0; i < w; ++i){
        for(j = 0; j < h; ++j){
            dest_index = 4*i + 4*w*j;
            bit[dest_index + 0 ] = ~(bit[dest_index + 0]);
            bit[dest_index + 1 ] = ~(bit[dest_index + 1]);
            bit[dest_index + 2 ] = ~(bit[dest_index + 2]);
        }
    }
    processed_img = processed_img.fromImage(proc);
    ui->proc_frame->setPixmap(processed_img.scaled(ui->proc_frame->size(), Qt::KeepAspectRatio));
    ui->status->setText("Image inversion done!");
    proc_stat |= inv_done;
}

void Interface::on_imgHistEql_clicked()
{
    if (check_status(hist_done))
        return;
    QImage proc = original_img.copy().toImage();
    uchar *bit = proc.bits();
    int h = proc.height();
    int w = proc.width();
    int pixels = w*h;
    int i, j, dest_index;
    float *crfR = new float[256] ();
    float *crfG = new float[256] ();
    float *crfB = new float[256] ();
    for(i = 0; i < w; ++i){
        for(j = 0; j < h; ++j){
            dest_index = 4*i + 4*w*j;
            crfR[static_cast<int>(bit[dest_index + 0])]++;
            crfG[static_cast<int>(bit[dest_index + 1])]++;
            crfB[static_cast<int>(bit[dest_index + 2])]++;
        }
    }
    for(i = 1; i <= 255; i++)
    {
        crfR[i] += crfR[i-1];
        crfG[i] += crfG[i-1];
        crfB[i] += crfB[i-1];
    }
    for(i = 0; i < w; ++i){
        for(j = 0; j < h; ++j){
            dest_index = 4*i + 4*w*j;
            bit[dest_index+0] = static_cast<uchar>(
                    static_cast<int>((crfR[static_cast<int>(bit[dest_index+0])] / pixels * 255)));
            bit[dest_index+1] = static_cast<uchar>(
                    static_cast<int>((crfG[static_cast<int>(bit[dest_index+1])] / pixels * 255)));
            bit[dest_index+2] = static_cast<uchar>(
                    static_cast<int>((crfB[static_cast<int>(bit[dest_index+2])] / pixels * 255)));
        }
    }
    delete[] crfR;
    delete[] crfG;
    delete[] crfB;
    processed_img = processed_img.fromImage(proc);
    ui->proc_frame->setPixmap(processed_img.scaled(ui->proc_frame->size(), Qt::KeepAspectRatio));
    ui->status->setText("Histogram equalization done!");
    proc_stat |= hist_done;
}

void Interface::on_imgGammaCor_clicked()
{
    if(check_status())
        return;
    QRegExp reg("[-+]?[0-9.]*");
    QString gamma_in = ui->gamma_input->text();
    double gamma_val;
    if(reg.exactMatch(gamma_in))
        gamma_val = gamma_in.toDouble();
    else{
        ui->status->setText("Please set a valid value for gamma");
        return;
    }
    QImage proc = original_img.copy().toImage();
    uchar *bit = proc.bits();
    int h = proc.height();
    int w = proc.width();
    int i, j, dest_index;
    for(i = 0; i < w; ++i){
        for(j = 0; j < h; ++j){
            dest_index = 4*i + 4*w*j;
            bit[dest_index+0] = static_cast<uchar>(
                    static_cast<int>(255 * pow(static_cast<double>(bit[dest_index+0]) / 255, gamma_val)));
            bit[dest_index+1] = static_cast<uchar>(
                    static_cast<int>(255 * pow(static_cast<double>(bit[dest_index+1]) / 255, gamma_val)));
            bit[dest_index+2] = static_cast<uchar>(
                    static_cast<int>(255 * pow(static_cast<double>(bit[dest_index+2]) / 255, gamma_val)));
        }
    }
    processed_img = processed_img.fromImage(proc);
    ui->proc_frame->setPixmap(processed_img.scaled(ui->proc_frame->size(), Qt::KeepAspectRatio));
    ui->status->setText("Gamma correction done! Input gamma value: " + QString::number(gamma_val));
    proc_stat |= gamma_done;
}


void Interface::on_imgReset_clicked()
{
    if(check_status())
        return;
    processed_img = original_img.copy();
    ui->proc_frame->setPixmap(processed_img.scaled(ui->proc_frame->size(), Qt::KeepAspectRatio));
    ui->status->setText("Image reset to default state.");
}

void Interface::on_actionSave_as_triggered()
{
    if(check_status())
        return;
    QString saveFileName = QFileDialog::getSaveFileName(this, "Save processed image as..", "output.jpg",
                                                        "jpg, png, bmp (*.jpg, *.png *.bmp)");
    QString ext = QFileInfo(saveFileName).suffix();
    QPixmap pixmap;
    if(ext=="jpg" || ext=="png" || ext=="bmp")
    {
        QFile outfile(saveFileName);
        if (!outfile.open(QFile::WriteOnly)){
            QMessageBox::warning(this, "Warning", "Cannot save file: " + outfile.errorString());
            return;
        }
        processed_img.save(&outfile);
        ui->status->setText("Saved processed image to: " + saveFileName);
    }
}

bool Interface::check_status(void)
{
    if(!static_cast<bool>(proc_stat & img_loaded))
    {
        QMessageBox::information(this, "Info", "Please load an image file first");
        return true;
    }
    proc_stat = 1;
    return false;
}

bool Interface::check_status(const uchar proc)
{
    if(static_cast<bool>(proc_stat & proc))
        return true;
    if(check_status())
        return true;
    return false;
}
