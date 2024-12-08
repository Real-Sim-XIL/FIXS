function VariantConfigs = getVariantSubsystemConfigs(MdlName)
% VariantConfigs = getVariantSubsystemConfigs(MdlName) returns
% vsr.mdl.SubsystemConfig objects for every variant in MdlName. Function
% currently does not support finding variants model references.


%% 
VariantSubsystems = find_system(MdlName, 'Variants', 'AllVariants', 'Variant', 'on');
idx = 1;
VariantConfigs = vsr.mdl.SubsystemConfig.empty;
for i = 1:length(VariantSubsystems)
    Variants = find_system(VariantSubsystems{i}, 'SearchDepth', 1, 'Variants', 'AllVariants', 'BlockType', 'SubSystem');
    Variants = Variants(~ismember(Variants, VariantSubsystems{i}));
    SelectedVariant = get_param(VariantSubsystems{i}, 'LabelModeActiveChoice');
    VariantControl = get_param(Variants, 'VariantControl');
    
    StartIdx = idx;
    for j = 1:length(Variants)
        VariantConfigs(idx) = vsr.mdl.SubsystemConfig(Variants{j});
        idx = idx + 1;
    end
    
    for j = (StartIdx + 1):length(VariantConfigs)
        VariantConfigs(StartIdx).addVariant(VariantConfigs(j));
    end
    
end

%% Model references
% RefMdls = find_mdlrefs(MdlName);



