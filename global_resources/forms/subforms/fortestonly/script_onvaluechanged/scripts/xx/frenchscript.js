function onFirstGroupComboValueChanged()
{
    forms.namespaceInUse = "Test::Script::OnValueChanged";
    var me = forms.item("FirstGroup::TestingCombo");
    var other = forms.item("FirstGroup::Result");
    other.currentText=me.currentText;
}

function onSecondGroupComboValueChanged()
{
    forms.namespaceInUse = "Test::Script::OnValueChanged";
    var me = forms.item("SecondGroup::TestingCombo");
    var other = forms.item("SecondGroup::Result");
    other.currentText += "<b>New value selected:</b> " + me.currentText + "<br />";
}
