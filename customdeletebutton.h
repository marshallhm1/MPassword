#ifndef CUSTOMDELETEBUTTON_H
#define CUSTOMDELETEBUTTON_H

#include <QPushButton>

class CustomDeleteButton : public QPushButton
{
    Q_OBJECT

public:
    CustomDeleteButton(QWidget* parent = nullptr);
    ~CustomDeleteButton();

signals:
    void deleteClicked(); // Signal emitted when the delete button is clicked

private slots:
    void onDeleteClicked(); // Slot to handle the delete button click

};

#endif // CUSTOMDELETEBUTTON_H
