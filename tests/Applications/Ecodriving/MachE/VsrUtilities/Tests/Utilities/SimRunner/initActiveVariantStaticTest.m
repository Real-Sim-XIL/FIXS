function initActiveVariantStaticTest(DataDictionaryName, ParamName, ParamValue)
DdObject = Simulink.data.dictionary.open(DataDictionaryName);
DdSectionObj = DdObject.getSection('Design Data');
DdEntry = DdSectionObj.find('Name', ParamName);
if ~isempty(DdEntry)
    DdEntry.setValue(ParamValue);
end
end