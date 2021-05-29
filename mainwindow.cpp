#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "view.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
    , ui{new Ui::MainWindow}
{
    ui->setupUi(this);
    ui->curDrawModeView->setText(lineChosenMsg);
    view = new View(defaultMWSize);
    ui->viewLayout->addWidget(new QLabel);  // For left padding
    ui->viewLayout->addWidget(view);
    ui->viewLayout->addWidget(new QLabel);  // For right padding

    penColorDialog = new QColorDialog(this);
    connect(penColorDialog, &QColorDialog::colorSelected, this, &MainWindow::on_penColorDialog_selected);

    view->getBrush().setColor(QColor(Qt::white));

    setWindowTitle('*' + mainWindowTitle);
    geometry();

    connect(view, &View::fileChange, this, &MainWindow::on_fileChange);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_S && (event->modifiers() & Qt::ControlModifier))
        on_actionSafe_triggered();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(view->isFileChange())
    {
        QMessageBox msg;
        msg.setText(unsafeTextMsg);
        msg.setWindowTitle(unsafeTitleMsg);
        msg.setIcon(QMessageBox::Question);
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int answer = msg.exec();

        if(answer == QMessageBox::Yes)
        {
            on_actionSafe_triggered();
        }
    }
    event->accept();
}

void MainWindow::on_toolBtnLine_clicked()
{
    view->setDrawMode(View::Line);
    ui->curDrawModeView->setText(lineChosenMsg);
}

void MainWindow::on_toolBtnRect_clicked()
{
    view->setDrawMode(View::Rect);
    ui->curDrawModeView->setText(rectChosenMsg);
}

void MainWindow::on_toolBtnEllipse_clicked()
{
    view->setDrawMode(View::Ellipse);
    ui->curDrawModeView->setText(ellipseChosenMsg);
}

void MainWindow::on_toolBtnPen_clicked()
{
    view->setDrawMode(View::JustPen);
    ui->curDrawModeView->setText(penChosenMsg);
}

void MainWindow::on_toolPolyBtn_clicked()
{
    view->setDrawMode(View::Poly);
    ui->curDrawModeView->setText(polyChosenMsg);
}

void MainWindow::on_typePenBox_activated(int index)
{
    switch (index)
    {
    case 0:
        view->getPen().setStyle(Qt::SolidLine);
        break;
    case 1:
        view->getPen().setStyle(Qt::DashLine);
        break;
    case 2:
        view->getPen().setStyle(Qt::DotLine);
        break;
    case 3:
        view->getPen().setStyle(Qt::DashDotLine);
        break;
    case 4:
        view->getPen().setStyle(Qt::DashDotDotLine);
        break;
    }
}

void MainWindow::on_sizePenBox_activated(int index)
{
    view->getPen().setWidth(index++);
}

void MainWindow::on_chooseColorPenButton_clicked()
{
    view->setTypeColor(View::Pen);
    penColorDialog->open();
}

void MainWindow::on_chooseColorBrushButton_clicked()
{
    view->setTypeColor(View::Brush);
    penColorDialog->open();
}

void MainWindow::on_penColorDialog_selected(const QColor &color)
{
    QPixmap forLabel(ui->penParamsLabel->size());
    forLabel.fill(color);
    if(view->getTypeColor() == View::Pen)
    {
        view->getPen().setColor(color);
        ui->colorPenLabel->setPixmap(forLabel);
    }
    else
    {
        view->getBrush().setColor(color);
        ui->colorBrushLabel->setPixmap(forLabel);
    }
}

void MainWindow::on_typeBrushBox_activated(int index)
{
    switch (index)
    {
    case 0:
        view->getBrush().setStyle(Qt::NoBrush);
        break;
    case 1:
        view->getBrush().setStyle(Qt::SolidPattern);
        break;
    case 2:
        view->getBrush().setStyle(Qt::HorPattern);
        break;
    case 3:
        view->getBrush().setStyle(Qt::VerPattern);
        break;
    case 4:
        view->getBrush().setStyle(Qt::CrossPattern);
        break;
    case 5:
        view->getBrush().setStyle(Qt::DiagCrossPattern);
        break;
    }
}


void MainWindow::on_actionOpen_triggered()
{
    QString tmpFileName;

    if(view->isFileChange())    // Confirm saving
    {
        QMessageBox msg;
        msg.setText(unsafeTextMsg);
        msg.setWindowTitle(unsafeTitleMsg);
        msg.setIcon(QMessageBox::Question);
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int answer = msg.exec();

        if(answer == QMessageBox::Yes)
        {
            on_actionSafe_triggered();
        }
    }

    tmpFileName = QFileDialog::getOpenFileName(this, "Choose image", "", "JPEG (*.jpg);; PNG (*.png)");

    if(!tmpFileName.isEmpty())
    {
        fileNameImg = tmpFileName;
        if(!view->getOrigPixmap().load(fileNameImg))
        {
            QMessageBox msg;
            msg.setText(errorOpenTextMsg);
            msg.setWindowTitle(errorOpenTitleMsg);
            msg.setIcon(QMessageBox::Warning);
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        }
        else
        {
            isOpenFile = true;
            view->getPixmap() = view->getOrigPixmap().scaled(defaultMWSize, Qt::KeepAspectRatio);
            view->setPixmap(view->getPixmap());

            view->setOrigRatio();
            setWindowTitle(fileNameImg);
        }
    }

}

void MainWindow::on_actionSafe_triggered()
{
    if(!isOpenFile)
    {
        fileNameImg = QFileDialog::getSaveFileName(this, "Save image", "", "Images (*.jpg *.png)");
        if(fileNameImg.isEmpty())
            return;

        QString fileSuffix = fileNameImg.split(".").last().toLower();
        if(fileSuffix != "jpg" && fileSuffix != "png")   // If user didn't enter type, add it automatically
            fileNameImg += ".png";
    }
    if(!view->getOrigPixmap().save(fileNameImg))
    {
        QMessageBox msg;
        msg.setText(errorSaveTextMsg);
        msg.setWindowTitle(errorSaveTitleMsg);
        msg.setIcon(QMessageBox::Warning);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
    else
    {
        isOpenFile = true;
        view->setChangedFile();   // Realese unsafe flag
        setWindowTitle(fileNameImg);
    }
}

void MainWindow::on_fileChange()
{
    if(!fileNameImg.isEmpty())
        setWindowTitle('*' + fileNameImg);
    else
        setWindowTitle('*' + mainWindowTitle);
}
