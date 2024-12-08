classdef MdlConfigApp < matlab.apps.AppBase
    
    %% Properties that correspond to app components
    properties (Access = public)
        MdlConfig                 matlab.ui.Figure
        MainGridLayout            matlab.ui.container.GridLayout
        GridLayout6               matlab.ui.container.GridLayout
        OKButton                  matlab.ui.control.Button
        CancelButton              matlab.ui.control.Button
        ApplyButton               matlab.ui.control.Button
        GridLayout4               matlab.ui.container.GridLayout
        ConfigTree                matlab.ui.container.Tree
        GridLayout5               matlab.ui.container.GridLayout
        ConfigfileEditFieldLabel  matlab.ui.control.Label
        ConfigFileEditField       matlab.ui.control.EditField
        SelectFileButton          matlab.ui.control.Button
        
        
    end
    
    %% private properties
    properties (SetAccess = private)
        MdlConfigBlk char
        MdlConfigObjs vsr.mdl.SimConfig
        LoadedConfigFileName char
    end
    %% Component initialization
    methods (Access = private)
        
        % Create UIFigure and components
        function createComponents(app)
            
            % Create MdlConfig and hide until all components are created
            app.MdlConfig = uifigure('Visible', 'off');
            app.MdlConfig.Position = [100 100 770 620];
            app.MdlConfig.Name = 'UI Figure';
            app.MdlConfig.UserData = app;
            
            % Create MainGridLayout
            app.MainGridLayout = uigridlayout(app.MdlConfig);
            app.MainGridLayout.ColumnWidth = {'1x'};
            app.MainGridLayout.RowHeight = {'1x', 45};
            
            % Create GridLayout6
            app.GridLayout6 = uigridlayout(app.MainGridLayout);
            app.GridLayout6.ColumnWidth = {'1x', 100, 100, 100};
            app.GridLayout6.RowHeight = {'1x'};
            app.GridLayout6.Layout.Row = 2;
            app.GridLayout6.Layout.Column = 1;
            
            % Create OKButton
            app.OKButton = uibutton(app.GridLayout6, 'push');
            app.OKButton.Layout.Row = 1;
            app.OKButton.Layout.Column = 2;
            app.OKButton.Text = 'OK';
            app.OKButton.ButtonPushedFcn = createCallbackFcn(app, @OKButtonPushed, true);
            
            % Create CancelButton
            app.CancelButton = uibutton(app.GridLayout6, 'push');
            app.CancelButton.Layout.Row = 1;
            app.CancelButton.Layout.Column = 3;
            app.CancelButton.Text = 'Cancel';
            app.CancelButton.ButtonPushedFcn = createCallbackFcn(app, @CancelButtonPushed, true);
            
            % Create ApplyButton
            app.ApplyButton = uibutton(app.GridLayout6, 'push');
            app.ApplyButton.Layout.Row = 1;
            app.ApplyButton.Layout.Column = 4;
            app.ApplyButton.Text = 'Apply';
            app.ApplyButton.ButtonPushedFcn = createCallbackFcn(app, @ApplyButtonPushed, true);
            
            % Create GridLayout4
            app.GridLayout4 = uigridlayout(app.MainGridLayout);
            app.GridLayout4.ColumnWidth = {'1x'};
            app.GridLayout4.RowHeight = {40, '1x'};
            app.GridLayout4.Layout.Row = 1;
            app.GridLayout4.Layout.Column = 1;
            
            % Create ConfigTree
            app.ConfigTree = uitree(app.GridLayout4);
            app.ConfigTree.Layout.Row = 2;
            app.ConfigTree.Layout.Column = 1;
            
            % Create GridLayout5
            app.GridLayout5 = uigridlayout(app.GridLayout4);
            app.GridLayout5.ColumnWidth = {75, '1x', 50};
            app.GridLayout5.RowHeight = {'1x'};
            app.GridLayout5.Layout.Row = 1;
            app.GridLayout5.Layout.Column = 1;
            
            % Create ConfigfileEditFieldLabel
            app.ConfigfileEditFieldLabel = uilabel(app.GridLayout5);
            app.ConfigfileEditFieldLabel.HorizontalAlignment = 'right';
            app.ConfigfileEditFieldLabel.Layout.Row = 1;
            app.ConfigfileEditFieldLabel.Layout.Column = 1;
            app.ConfigfileEditFieldLabel.Text = 'Config file:';
            
            % Create ConfigFileEditField
            app.ConfigFileEditField = uieditfield(app.GridLayout5, 'text');
            app.ConfigFileEditField.Layout.Row = 1;
            app.ConfigFileEditField.Layout.Column = 2;
            app.ConfigFileEditField.Value = get_param(app.MdlConfigBlk, 'ConfigFileName');
            
            % Create SelectFileButton
            app.SelectFileButton = uibutton(app.GridLayout5, 'push');
            app.SelectFileButton.Layout.Row = 1;
            app.SelectFileButton.Layout.Column = 3;
            app.SelectFileButton.Text = 'Browse';
            
            % Show the figure after all components are created
            app.MdlConfig.Visible = 'on';
        end
    end
    
    %% App creation and deletion
    methods (Access = public)
        
        % Construct app
        function app = MdlConfigApp(BlkName)
            app.MdlConfigBlk = BlkName;
            % Create UIFigure and components
            createComponents(app)
            
            app.updateConfigTree;
            
            app.MdlConfig.Name = ['Model Configuration: ', bdroot(BlkName)];
            
            %             % Register the app with App Designer
            %             registerApp(app, app.MdlConfig)
            
            if nargout == 0
                clear app
            end
        end
        
        % Code that executes before app deletion
        function delete(app)
            
            % Delete UIFigure when app is deleted
            delete(app.MdlConfig)
        end
    end
    
    %% Callbacks that handle component events
    methods (Access = private)
        
        % Button pushed function: ApplyButton
        function ApplyButtonPushed(app, event)
            set_param(app.MdlConfigBlk, 'ConfigFileName', app.ConfigFileEditField.Value)
            app.updateConfigTree(false);
            SelectedNodes = app.ConfigTree.SelectedNodes;
            for i = 1:length(SelectedNodes)
                if ~isempty(SelectedNodes(i).NodeData)
                    SelectedNodes(i).NodeData.activateVariant;
                end
            end
            app.updateConfigTree;
        end
        
        % Button pushed function: OkButton
        function OKButtonPushed(app, event)
            app.ApplyButtonPushed(event)
            app.delete;
        end
        
        % Button pushed function: OkButton
        function CancelButtonPushed(app, event)
            
            app.delete;
        end
        
    end
    
    %% Private methods
    methods (Access = private)
        %% updateConfigTree
        function updateConfigTree(app, RefreshIcon)
            if nargin < 2
                RefreshIcon = true;
            end
            % Setup
            ConfigFileName = app.ConfigFileEditField.Value;
            if strcmp(ConfigFileName, app.LoadedConfigFileName)
                if RefreshIcon
                    app.refreshTreeIcons(app.ConfigTree)
                end
            elseif ~isempty(ConfigFileName) || ~strcmp(ConfigFileName, app.LoadedConfigFileName)
                try
                    ConfigObjs = eval(ConfigFileName);
                    app.MdlConfigObjs = ConfigObjs;
                    app.LoadedConfigFileName = ConfigFileName;
                catch
                    warning('Config file ''%s'' could not be evaluated.', ConfigFileName)
                    ConfigObjs = [];
                end
                % Add nodes to tree
                for i = 1:length(ConfigObjs)
                    createConfigNode(app, ConfigObjs(i), app.ConfigTree, i);
                end
                if RefreshIcon
                    app.refreshTreeIcons(app.ConfigTree)
                end
            else
                ConfigObjs = [];
            end
            
        end
        
        
        %% createConfigNode
        function createConfigNode(app, ConfigObj, ParentTreeNode, Index)
            ParentChildren = ParentTreeNode.Children;
            if Index > length(ParentChildren)
                CurrNode = uitreenode(ParentTreeNode);
            else
                CurrNode = ParentChildren(Index);
            end
            
            CurrNode.Text = ConfigObj.Label;
            CreateSubNodes = false;
            if isa(ConfigObj, 'vsr.mdl.SimConfigSet')
                if ConfigObj.IndependentConfigs
                    CreateSubNodes = true;
                end
            end
            if CreateSubNodes
                for i = 1:length(ConfigObj.Configs)
                    app.createConfigNode(ConfigObj.Configs(i), CurrNode, i)
                end
            else
                CurrNode.NodeData = ConfigObj;  
            end
            
        end
        
        %% refreshTreeIcons
        function refreshTreeIcons(app, TreeNodes)
            for i = 1:length(TreeNodes)
                if isempty(TreeNodes(i).Children)
                    ConfigObj = TreeNodes(i).NodeData;
                    if ConfigObj.isActive
                        TreeNodes(i).Icon = 'vsr_mdl_config_active.png';
                    else
                        TreeNodes(i).Icon = '';
                    end
                else
                    app.refreshTreeIcons(TreeNodes(i).Children)
                end
            end
        end
    end
end


