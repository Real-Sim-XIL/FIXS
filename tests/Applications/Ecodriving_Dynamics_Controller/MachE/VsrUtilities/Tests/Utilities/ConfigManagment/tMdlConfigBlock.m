classdef tMdlConfigBlock < matlab.unittest.TestCase
    
    properties
        MdlName = 'mConfigBlk';
        ConfigBlkName = 'mConfigBlk/Model Configuration';
    end
    
    %% TestMethodTeardown
    methods (TestMethodTeardown)
        %% closeMdls
        function closeMdls(obj)
            close_system(obj.MdlName, 0);
        end
    end
    
    %% Test methods
    methods (Test)
        %% verifyBlockOpen
        function verifyBlockOpen(obj)
            open_system(obj.MdlName)
            open_system(obj.ConfigBlkName)
            FoundFig = findall(0, 'HandleVisibility', 'off');
            FoundFig = FoundFig(strcmp({FoundFig.Name}, 'Model Configuration: mConfigBlk'));
            AppObj = FoundFig.UserData;
            
            AppObj.ApplyButton.ButtonPushedFcn
            AppObj.OKButton.ButtonPushedFcn([],[])
            close_system(obj.MdlName, 0);
        end
        
    end
    
end