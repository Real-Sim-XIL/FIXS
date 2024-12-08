classdef tMappedEngAutonomieTransfer < matlab.unittest.TestCase
    
   
    %% Public properties
    properties
        DataDictionaryName = 'DdMappedEngAutonomieTest';
        MappedEngMdlName = 'mMappedEngAutonomieData';
    end
    
    %% Test parameter
    properties (TestParameter)
        AutonomieFile = {'eng_plant_ci_Cummins_ISX450_CAEC_V2'; ...
            'eng_plant_cng_8100_186_JohnDeere';...
            'eng_plant_gasoline_1pt6L_engine_turbo';...
            'eng_plant_si_6000_226'};
    end
    
    %% TestMethodSetup
    methods (TestMethodSetup)
       %% addAutonomieFilesToPath
       function addAutonomieFilesToPath(obj)
            addpath(fullfile(fileparts(which(mfilename)), 'AutonomieInitFiles'))
       end
    end
    %% TestMethodTeardown
    methods (TestMethodTeardown)
        %% discardDataDictionaryChanges
        function discardDataDictionaryChanges(obj)
            DataDictionaryObj = vsr.mdl.DataDictionaryConfig;
            DataDictionaryObj.DataDictionaryName = obj.DataDictionaryName;
            DataDictionaryObj.DataDictionary.discardChanges;
            DataDictionaryObj.DataDictionary.close;
        end
        
        %% removeAutonomieFilesFromPath
        function removeAutonomieFilesFromPath(obj)
            rmpath(fullfile(fileparts(which(mfilename)), 'AutonomieInitFiles'))
        end
    end
    
    %% Test methods
    methods (Test)
        
        %% verifyUpdateMappedEngDataFromAutonomie
        function verifyUpdateMappedEngDataFromAutonomie(obj, AutonomieFile)
           % Update data dictionary
           EngDataObj = vsr.mdl.eng.updateEngDdFromAutonomie(AutonomieFile, obj.DataDictionaryName);
           
           % Run simulation to check for errors (Results could be verified
           % later)
           open_system(obj.MappedEngMdlName)
           sim(obj.MappedEngMdlName);
           close_system((obj.MappedEngMdlName), 0)
        end
        
        
    end
    
end