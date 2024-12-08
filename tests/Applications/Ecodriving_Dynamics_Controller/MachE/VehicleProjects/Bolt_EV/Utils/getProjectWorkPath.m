function WorkFolderPath = getProjectWorkPath
%getProjectWorkPath Function returns the path of the project work folder.
%All files in the work folder are derived.

project = simulinkproject;
projectRoot = project.RootFolder;

WorkFolderPath = fullfile(projectRoot, 'Work');
end

