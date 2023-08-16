#include "MPassword.h"
#include "ui_MPassword.h"
#include "customdeletebutton.h"
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QRandomGenerator>
#include <QClipboard>
#include <QCryptographicHash>


QString xorEncodeDecode(const QString &input, const QString &key) {
    QString result;
    for (int i = 0; i < input.size(); ++i) {
        QChar encodedChar = QChar(input.at(i).unicode() ^ key.at(i % key.size()).unicode());
        result.append(encodedChar);
    }
    return result;
}

void clearPasswordsFile()
{
    QFile file("passwords.txt");
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        // Read the first line (master password) and store it in a variable
        QTextStream in(&file);
        QString masterPassword = in.readLine().trimmed();

        // Clear the file content
        file.resize(0);

        // Write back the master password to the file
        if (!masterPassword.isEmpty()) {
            file.seek(0); // Move to the beginning of the file
            QTextStream out(&file);
            out << masterPassword << "\n";
        }
        file.close();
    }
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MPasswordManager)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(3);
    ui->dockWidget->hide();

    // Read and decrypt the master password from the file
    QString xorKey = "w7T9xK#pD2*5jL$Z";
    QString encryptedMasterPassword;

    QFile file("passwords.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        encryptedMasterPassword = in.readLine().trimmed();
        file.close();
    }

    if (!encryptedMasterPassword.isEmpty()) {
        QString decryptedMasterPassword = xorEncodeDecode(encryptedMasterPassword, xorKey);
        ui->masterPasswordLineEdit->setText(decryptedMasterPassword);
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}


struct PasswordData {
    QString label;
    QString username;
    QString password;
};

void MainWindow::showPlaceholderMessage(const QString& message)
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(1);
    ui->tableWidget->setColumnCount(1);
    QTableWidgetItem* placeholderItem = new QTableWidgetItem(message);
    placeholderItem->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(0, 0, placeholderItem);

    // Resize the first column to fit the content (placeholder message)
    ui->tableWidget->resizeColumnToContents(0);
}


QList<PasswordData> readPasswordsFromFile(const QString &filename, const QString &xorKey) {
    QList<PasswordData> passwords;
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            // Parse the line to extract the encoded label, username, and password
            int labelIndex = line.indexOf("Label: ") + QString("Label: ").length();
            int usernameIndex = line.indexOf("Username: ") + QString("Username: ").length();
            int passwordIndex = line.indexOf("Password: ") + QString("Password: ").length();

            QString encodedLabel = line.mid(labelIndex, usernameIndex - labelIndex - QString("Username: ").length()).trimmed();
            QString encodedUsername = line.mid(usernameIndex, passwordIndex - usernameIndex - QString("Password: ").length()).trimmed();
            QString encodedPassword = line.mid(passwordIndex).trimmed();

            // Decode the encoded label, username, and password using XOR decoding
            QString label = xorEncodeDecode(encodedLabel, xorKey);
            QString username = xorEncodeDecode(encodedUsername, xorKey);
            QString password = xorEncodeDecode(encodedPassword, xorKey);

            // Additional check to ensure label, username, and password are not empty
            if (!label.isEmpty() && !username.isEmpty() && !password.isEmpty()) {
                PasswordData data;
                data.label = label;
                data.username = username;
                data.password = password;
                passwords.append(data);
            }
        }
        file.close();
    }
    return passwords;
}



void MainWindow::on_actionHome_triggered()

{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_actionSave_New_Password_triggered()
{
    ui->stackedWidget->setCurrentIndex(1);

}


QString generateRandomPassword(int length)
{
    QString validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    QString password;
    for (int i = 0; i < length; ++i) {
        int randomIndex = QRandomGenerator::global()->bounded(validChars.length());
        QChar randomChar = validChars.at(randomIndex);
        password.append(randomChar);
    }

    return password;
}



void removePasswordFromFile(const QString& label, const QString& username, const QString& password)
{
    QFile file("passwords.txt");
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream in(&file);
        QStringList fileContent;

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed(); // Trim the line to handle empty lines correctly
            if (line.isEmpty()) {
                continue; // Skip empty lines
            }

            QString lineLabel = line.mid(line.indexOf("Label: ") + QString("Label: ").length(),
                                         line.indexOf(" Username: ") - line.indexOf("Label: ") - QString("Label: ").length());
            QString lineUsername = line.mid(line.indexOf("Username: ") + QString("Username: ").length(),
                                            line.indexOf(" Password: ") - line.indexOf("Username: ") - QString("Username: ").length());
            QString linePassword = line.mid(line.indexOf("Password: ") + QString("Password: ").length());

            if (lineLabel != label || lineUsername != username || linePassword != password) {
                fileContent << line;
            }
        }
        file.resize(0);
        QTextStream out(&file);
        out << fileContent.join("\n");
        file.close();
    }
}



void MainWindow::deletePassword(int rowIndex)
{
    if (rowIndex >= 0 && rowIndex < ui->tableWidget->rowCount()) {
        // Get the label, username, and password from the selected row
        QTableWidgetItem* labelItem = ui->tableWidget->item(rowIndex, 0);
        QTableWidgetItem* usernameItem = ui->tableWidget->item(rowIndex, 1);
        QTableWidgetItem* passwordItem = ui->tableWidget->item(rowIndex, 2);

        QString label = labelItem->text();
        QString username = usernameItem->text();
        QString password = passwordItem->text();

        // Check if the row is empty (no label, username, and password)
        bool rowIsEmpty = (label.isEmpty() && username.isEmpty() && password.isEmpty());

        ui->tableWidget->removeRow(rowIndex);

        if (!rowIsEmpty) {
            // Update the file to remove the deleted entry if the row was not empty
            removePasswordFromFile(label, username, password);
        }

        if (ui->tableWidget->rowCount() == 0 || rowIsEmpty) {
            showPlaceholderMessage("No passwords saved.");
        }
    }
}



void MainWindow::on_actionView_Saved_Password_triggered()
{
    // Read the passwords from the file
    QString xorkey = "w7T9xK#pD2*5jL$Z";
    QList<PasswordData> passwords = readPasswordsFromFile("passwords.txt", xorkey);

    // If there are no passwords, show the placeholder message and set the table widget row count to 0
    if (passwords.isEmpty()) {
        showPlaceholderMessage("No passwords saved.");
        ui->stackedWidget->setCurrentIndex(2);
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);
        return;
    }

    // If there are passwords, proceed to display them in the table
    ui->stackedWidget->setCurrentIndex(2);

    // Display the passwords in a QTableWidget
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(passwords.size());

    ui->tableWidget->setRowCount(passwords.size());
    ui->tableWidget->setColumnCount(4); // Adjust the column count to include the "Label" column

    // Set the table headers (column labels)
    QStringList headers;
    headers << "Label" << "Username" << "Password" << "Delete";
    ui->tableWidget->setHorizontalHeaderLabels(headers);



    int row = 0;
    for (const PasswordData& data : passwords) {
        QTableWidgetItem* nicknameItem = new QTableWidgetItem(data.label); // Add this line for nickname/website
        QTableWidgetItem* usernameItem = new QTableWidgetItem(data.username);
        QTableWidgetItem* passwordItem = new QTableWidgetItem(data.password);

        // Set text alignment for the cells
        nicknameItem->setTextAlignment(Qt::AlignCenter); // Align the new column
        usernameItem->setTextAlignment(Qt::AlignCenter);
        passwordItem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget->setItem(row, 0, nicknameItem); // Set the new column item
        ui->tableWidget->setItem(row, 1, usernameItem);
        ui->tableWidget->setItem(row, 2, passwordItem);

        if (!data.password.isEmpty()) {
            CustomDeleteButton* customDeleteWidget = new CustomDeleteButton(this);
            connect(customDeleteWidget, &CustomDeleteButton::deleteClicked, this, [this, row]() {
                deletePassword(row);
            });

            // Set the custom widget as the cell widget for the fourth column (index 3)
            ui->tableWidget->setCellWidget(row, 3, customDeleteWidget);

            // Set a minimum width for the "Delete" column
            ui->tableWidget->setColumnWidth(3, 100);
        } else {
            ui->tableWidget->setCellWidget(row, 3, nullptr);
        }

        row++;
    }

    // Resize the columns to fit the content
    ui->tableWidget->resizeColumnsToContents();
}



void MainWindow::on_pushButton_clicked() {
    // Get the entered username and password from the QLineEdit widgets
    QString label = ui->labelLineEdit->text();
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    // Define a key for XOR encoding (choose a random key)
    QString xorKey = "w7T9xK#pD2*5jL$Z";

    // Encode the username and password using XOR encoding
    QString encodedLabel = xorEncodeDecode(label, xorKey);
    QString encodedUsername = xorEncodeDecode(username, xorKey);
    QString encodedPassword = xorEncodeDecode(password, xorKey);

    // Open the file in write-only mode and append data at the end (create if it doesn't exist)
    QFile file("passwords.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Label: " << encodedLabel << "Username: " << encodedUsername << " Password: " << encodedPassword << "\n";
        file.close();

        ui->password_saved_label->setText("Password saved successfully.");
    } else {
        ui->password_saved_label->setText("Failed to save password.");
    }
}




void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    // Read the passwords from the file
    QString xorkey = "w7T9xK#pD2*5jL$Z";
    QList<PasswordData> passwords = readPasswordsFromFile("passwords.txt", xorkey);

    // Display the passwords in a QTableWidget
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(passwords.size());
    ui->tableWidget->setColumnCount(4); // Four columns: label, username, password, and delete button

    // Set the table headers (column labels)
    QStringList headers;
    headers << "Label" << "Username" << "Password" << "Delete";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    int row = 0;
    for (const PasswordData& data : passwords) {
        QTableWidgetItem* labelItem = new QTableWidgetItem(data.label); // Add this line for the label column
        QTableWidgetItem* usernameItem = new QTableWidgetItem(data.username);
        QTableWidgetItem* passwordItem = new QTableWidgetItem(data.password);

        // Set text alignment for the cells
        labelItem->setTextAlignment(Qt::AlignCenter);
        usernameItem->setTextAlignment(Qt::AlignCenter);
        passwordItem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget->setItem(row, 0, labelItem); // Set the label item
        ui->tableWidget->setItem(row, 1, usernameItem);
        ui->tableWidget->setItem(row, 2, passwordItem);

        if (!data.password.isEmpty()) {
            CustomDeleteButton* customDeleteWidget = new CustomDeleteButton(this);
            connect(customDeleteWidget, &CustomDeleteButton::deleteClicked, this, [this, row]() {
                deletePassword(row);
            });

            // Set the custom widget as the cell widget for the fourth column (index 3)
            ui->tableWidget->setCellWidget(row, 3, customDeleteWidget);

            // Set a minimum width for the "Delete" column
            ui->tableWidget->setColumnWidth(3, 100);
        } else {
            ui->tableWidget->setCellWidget(row, 3, nullptr);
        }

        row++;
    }

    // Resize the columns to fit the content
    ui->tableWidget->resizeColumnsToContents();
}







void MainWindow::on_pushButton_5_clicked()
{
    QString enteredPassword = ui->passwordLineEdit_2->text();

    // Define a key for XOR encoding (choose the same key as used for encryption)
    QString xorKey = "w7T9xK#pD2*5jL$Z";

    // Check if a master password exists in the file
    QFile file("passwords.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString encodedMasterPassword = in.readLine().trimmed();
        file.close();

        if (encodedMasterPassword.isEmpty()) {
            // No master password set, so set the new password and save it to the file
            QFile outFile("passwords.txt");
            if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&outFile);
                // Encrypt the entered password before saving
                QString encodedEnteredPassword = xorEncodeDecode(enteredPassword, xorKey);
                out << encodedEnteredPassword << "\n";
                outFile.close();

                // Switch to the main window
                ui->stackedWidget->setCurrentIndex(1);

                // Update the label to indicate that a master password has been set
                ui->statusbar->showMessage("Master password set successfully.", 3000);
                // Show and bring the dock widget to the top of the stacking order
                ui->dockWidget->show();
                ui->dockWidget->raise(); // Bring the dock widget to the top
            } else {
                // Show an error message if unable to save the password
                ui->statusbar->showMessage("Error: Failed to set master password.", 3000);
            }
        } else {
            // Master password exists, validate the entered password
            QString decodedMasterPassword = xorEncodeDecode(encodedMasterPassword, xorKey);
            if (enteredPassword == decodedMasterPassword) {
                // Password matches, switch to the main window
                ui->stackedWidget->setCurrentIndex(0);

                // Update the label to indicate that the correct master password was entered
                ui->statusbar->showMessage("Master password entered successfully.", 3000);
                // Show and bring the dock widget to the top of the stacking order
                ui->dockWidget->show();
                ui->dockWidget->raise(); // Bring the dock widget to the top
            } else {
                // Show an error message for incorrect password
                ui->statusbar->showMessage("Invalid password. Please try again.", 3000);
            }
        }
    } else {
        // Show an error message if unable to read the file
        ui->statusbar->showMessage("Error: Unable to read master password.", 3000);
    }
}


void clearAllPasswordsFile()
{
    QFile file("passwords.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        file.close();
    }
}


void MainWindow::on_pushButton_6_clicked()
{
    clearPasswordsFile();
}


void MainWindow::on_pushButton_9_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);

}


void MainWindow::on_pushButton_7_clicked()
{
    QString generatedPassword = generateRandomPassword(15);

    // Display the generated password in the QLabel
    ui->generatedPasswordLineEdit->setText(generatedPassword);
}


void MainWindow::on_pushButton_8_clicked()
{
    // Get the generated password from the QLabel
    QString generatedPassword = ui->generatedPasswordLineEdit->text();

    // Copy the generated password to the clipboard
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(generatedPassword);

    // Optionally, show a message or perform any other action after copying to clipboard
    // For example:
    ui->statusbar->showMessage("Password copied to clipboard!", 3000);
}

void MainWindow::on_pushButton_10_clicked()
{   // Wipe file
    clearAllPasswordsFile();
    // Refresh table
    on_pushButton_4_clicked();
}



void MainWindow::on_pushButton_11_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}


void MainWindow::on_pushButton_12_clicked()
{
    clearAllPasswordsFile();
}


void MainWindow::on_pushButton_13_clicked()
{
    // Get the new master password from the QLineEdit widget
    QString newMasterPassword = ui->masterPasswordLineEdit->text();

    // Define a key for XOR encoding
    QString xorKey = "w7T9xK#pD2*5jL$Z";

    // Encode the new master password using XOR encoding
    QString encodedMasterPassword = xorEncodeDecode(newMasterPassword, xorKey);

    // Open the file in write-only mode to save the new master password
    QFile file("passwords.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << encodedMasterPassword << "\n";
        file.close();

        // Optionally, you can show a message to indicate that the master password has been changed
        ui->statusbar->showMessage("Master password changed successfully.", 3000);
    } else {
        // Show an error message if unable to save the new master password
        ui->statusbar->showMessage("Error: Failed to change master password.", 3000);
    }
}


