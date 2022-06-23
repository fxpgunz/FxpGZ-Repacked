#include "QtDemoObjectRollupWidget.h"
#include "ui_QtPluginRollup.h"

/* This is a wrapper class for our Qt widget, defined in Qt_params.ui, and created in Qt Designer.*/


QtDemoObjectRollupWidget::QtDemoObjectRollupWidget(QWidget* parent)
	:QMaxParamBlockWidget(/*parent*/),
	ui(new Ui::QtPluginRollup())
{
	// At a minimum, we must call setupUi().  Other initialization can happen here.
	ui->setupUi(this);

}

// This is a widget that is not wired to a paramblock.  It can still be used via the Qt Signal/Slot mechanism.
void QtDemoObjectRollupWidget::QtDemoObjectRollupWidget::on_hibutton_clicked() {
	// We can access all the functionality in QtCore and QtGui, such as using various
	// dialogs.
	QMessageBox::information(this, "Hi", "Hi from Qt.");
}


QtDemoObjectRollupWidget::~QtDemoObjectRollupWidget()
{
	delete ui;
}


