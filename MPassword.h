#ifndef MPASSWORD_H
#define MPASSWORD_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MPasswordManager; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();



private slots:

    void deletePassword(int rowIndex);

    void on_actionHome_triggered();

    void on_actionSave_New_Password_triggered();

    void on_actionView_Saved_Password_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();


    void on_pushButton_6_clicked();
    void showPlaceholderMessage(const QString& message);

    void on_pushButton_9_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();



    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();





private:
    Ui::MPasswordManager *ui;




};
#endif // MPASSWORD_H
