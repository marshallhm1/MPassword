#include "customdeletebutton.h"

CustomDeleteButton::CustomDeleteButton(QWidget* parent)
    : QPushButton(parent)
{
    setText("Delete");
    connect(this, &QPushButton::clicked, this, &CustomDeleteButton::onDeleteClicked);
}

CustomDeleteButton::~CustomDeleteButton()
{
}

void CustomDeleteButton::onDeleteClicked()
{
    emit deleteClicked(); // Emit the deleteClicked signal when the button is clicked
}
