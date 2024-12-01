classdef tMappedEngConfig < matlab.unittest.TestCase
    
    %% Public properties
    properties
        DataDictionaryName = 'DdMappedEng';
        MappedEngMdlName = 'mMappedEng';
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
    end
    
    %% Test methods
    methods (Test)
        
        %% verifyParamSummaryTbl
        function verifyParamSumamryTbl(obj)
            EngConfigObj = vsr.mdl.eng.MappedEngConfig;
            EngConfigObj.DataDictionaryName = obj.DataDictionaryName;
            DataSummaryTbl = EngConfigObj.getParamSummaryTbl;
            
            % Check table
            obj.verifyFalse(all([DataSummaryTbl.isEqual{:}]))
            disp(DataSummaryTbl)
        end
        
        %% verifyPushToDataDictionary
        function verifyPushToDataDictionary(obj)
            % Create object
            EngConfigObj = vsr.mdl.eng.MappedEngConfig;
            EngConfigObj.DataDictionaryName = obj.DataDictionaryName;
            DataSummaryTbl = EngConfigObj.getParamSummaryTbl;
            
            % Check status of values
            obj.verifyFalse(all([DataSummaryTbl.isEqual{:}]))
            
            % Push parameters to data dictionary
            EngConfigObj.pushToDictionary;
            DataSummaryTbl = EngConfigObj.getParamSummaryTbl;
            disp('Parameter information after push:')
            disp(DataSummaryTbl)
            obj.verifyTrue(all([DataSummaryTbl.isEqual{:}]))
        end
        
        
    end
    
end