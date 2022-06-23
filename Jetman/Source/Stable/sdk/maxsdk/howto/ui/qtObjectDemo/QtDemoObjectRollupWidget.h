#pragma once
#include <maxapi.h>
#include <Qt/QMaxParamBlockWidget.h>

namespace Ui {
	class QtPluginRollup;
}

class QtDemoObjectRollupWidget : public MaxSDK::QMaxParamBlockWidget
{
	// This is a macro that connects this class to the Qt build system
	Q_OBJECT

public:
	explicit QtDemoObjectRollupWidget(QWidget* parent = nullptr);
	~QtDemoObjectRollupWidget(void);

	// Required by QMaxParamBlockWidget:
	virtual void SetParamBlock(ReferenceMaker* /*owner*/, IParamBlock2* const /*param_block*/){};
	virtual void UpdateUI(const TimeValue t){
		Interface* ip = GetCOREInterface();
		ip->RedrawViews(t, REDRAW_NORMAL);
	};
	virtual void UpdateParameterUI(const TimeValue /*t*/, const ParamID /*param_id*/, const int /*tab_index*/){};

protected slots:
	void on_hibutton_clicked();

private: 
	Ui::QtPluginRollup* ui;

};

