#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QString>
#include <QStringList>
#include <QMainWindow>
#include <QPixmap>
#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include "view.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    const QString mainWindowTitle{"Unnamed"};

    const QString lineChosenMsg{"Line was choosen"};
    const QString rectChosenMsg{"Rect was choosen"};
    const QString ellipseChosenMsg{"Ellipse was choosen"};
    const QString penChosenMsg{"Simple pen was choosen"};
    const QString polyChosenMsg{"Polygon was choosen (press right button to stop drawing)"};

    const QString errorOpenTitleMsg{"Error opening file"};
    const QString errorOpenTextMsg{"Error opening file. Try again."};
    const QString errorSaveTextMsg{"Error saving file. Try again."};
    const QString errorSaveTitleMsg{"Error saving file"};

    const QString unsafeTitleMsg{"Unsafed file!"};
    const QString unsafeTextMsg{"Do you want to safe your changes?"};

    void keyPressEvent(QKeyEvent *event) override;  // How can it be without Ctrl + S)
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_toolBtnLine_clicked();
    void on_toolBtnRect_clicked();
    void on_toolBtnEllipse_clicked();
    void on_typePenBox_activated(int index);
    void on_sizePenBox_activated(int index);
    void on_chooseColorPenButton_clicked();
    void on_penColorDialog_selected(const QColor&);
    void on_chooseColorBrushButton_clicked();
    void on_typeBrushBox_activated(int index);
    void on_toolBtnPen_clicked();
    void on_toolPolyBtn_clicked();

    void on_actionOpen_triggered();
    void on_actionSafe_triggered();

    void on_fileChange();

private:
    const QSize defaultMWSize{1280, 720};

    Ui::MainWindow *ui;
    QColorDialog* penColorDialog;
    View* view;

    QString fileNameImg;
    bool isOpenFile{false};
};
#endif // MAINWINDOW_H
