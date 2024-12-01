classdef TaskRunner
    % TaskRunner is used to run tasks in parallel and save results for each
    % task.
    
    %% Public properties
    properties
        Tasks vsr.taskrunner.Task; % Tasks to run
        ConfigSets vsr.mdl.SimConfigSet % Run simulation for each configuration
        UseParallel logical = false;
        %         UseFastRestart boolean = false;
    end
    
    %% Public methods
    methods
        
        %% run
        function obj = run(obj)
            if obj.UseParallel
%                 TaskList = obj.Tasks.copy;
                % Grant each worker in the parallel pool an independent data dictionary
                % so they can use the data without interference
                
                spmd
                    Simulink.data.dictionary.cleanupWorkerCache
                end
                spmd
                    Simulink.data.dictionary.setupWorkerCache
                end
                Vals = 1:length(obj.Tasks);
                FcnHdls = cell(size(obj.Tasks));
                for i = 1:numel(obj.Tasks)
                    TaskList{i} = obj.Tasks(i);
                    FcnHdls{i} = @() vsr.mdl.Param.initActiveVariantStatic('DdTestSimRunner.sldd', 'Var1',Vals(i));
                end
%                 FcnHdls = {obj.Tasks.ConfigSetFcnHdl};
                LoopTasks = obj.Tasks;
                parfor (i = 1:numel(TaskList), 'debug')
                    LoopTasks(i) = LoopTasks(i).runTask;
%                     DdObject = Simulink.data.dictionary.open('DdTestSimRunner.sldd');
%                     DdSectionObj = DdObject.getSection('Design Data');
%                     DdEntry = DdSectionObj.find('Name', 'Var1');
%                     DdEntry.setValue(Vals(i))
%                     vsr.mdl.Param.initActiveVariantStatic('DdTestSimRunner.sldd', 'Var1',Vals(i))
%                     FcnHdls{i}()
%                     CurrTask = TaskList{i};
%                     CurrTask.runTask;
%                     TaskList{i} = CurrTask;
                end
                obj.Tasks = LoopTasks;
%                 for i = 1:numel(obj.Tasks)
%                     obj.Tasks(i) =  TaskList{i};
%                 end
                spmd
                    Simulink.data.dictionary.cleanupWorkerCache
                end
            else
                for i = 1:numel(obj.Tasks)
                    obj.Tasks(i) = obj.Tasks(i).runTask;
                end
            end
        end
        
        %% addTask
        function addTask(obj, NewTask)
            obj.Tasks = [obj.Tasks, NewTask];
        end
    end
end