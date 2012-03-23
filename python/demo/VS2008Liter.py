#coding=utf-8
import XswUtility
import os
import sys

#精简vs2008，提高运行速度




if __name__ == "__main__":
	#当前执行的py文件绝对路径
	cur_path = os.path.dirname(sys.argv[0])

	#vs2008目录
	vs2008_path = r'D:\Program Files\Microsoft Visual Studio 9.0'

	#要精简的文件
	lite_files = [
	r'Common7\Packages\mswebprj.dll',
	r'Common7\Packages\WebAppUpgrade.dll',
	r'Common7\Packages\webmigration.dll',

	r'Common7\IDE\Microsoft.VisualStudio.Web.Application.dll',
	r'Common7\IDE\Microsoft.WebPublisher.dll',
	r'Common7\IDE\Microsoft.VisualStudio.CSharp.Options.dll',
	r'Common7\IDE\Microsoft.VisualStudio.CSharp.Services.Language.dll',
	r'Common7\IDE\srcsrv.dll',
	r'Common7\IDE\csformatui.dll',
	r'Common7\IDE\Microsoft.VisualBasic.UpgradeEngineInterface.dll',
	r'Common7\IDE\Microsoft.VisualBasic.UpgradeSnippet.dll',
	r'Common7\IDE\Microsoft.VisualBasic.UpgradeWizard.dll',
	r'Common7\IDE\Visual Basic 6.vsk',
	r'Common7\IDE\Visual C# 2005.vsk',
	r'Common7\IDE\Visual C++ 2.vsk',
	r'Common7\IDE\Brief.vsk',
	r'Common7\IDE\Emacs.vsk',

	r'Common7\IDE\VS SCC\msemp.dll',
	r'Common7\IDE\VS SCC\VssProviderStub.dll',
	r'Common7\IDE\VS SCC\VssProvider.dll',

	r'Common7\IDE\VS SCC\1033\msempui.dll',
	r'Common7\IDE\VS SCC\1033\VssProviderStubui.dll',
	r'Common7\IDE\VS SCC\1033\VssProviderui.dll',

	r'Common7\IDE\Microsoft.VisualStudio.vspWmiEnum.dll',
	r'Common7\IDE\Microsoft.VisualStudio.Web.dll',
	r'Common7\IDE\Microsoft.VisualStudio.WPFFlavor.dll',
	r'Common7\IDE\Microsoft.VSDesigner.Management.dll',
	r'Common7\IDE\Microsoft.Web.Design.Client.dll',
	r'Common7\IDE\Microsoft.Workflow.DebugController.dll',
	r'Common7\IDE\Microsoft.Workflow.ExpressionEvaluation.dll',
	r'Common7\IDE\Microsoft.Workflow.ExpressionEvaluation.pdb',
	r'Common7\IDE\Microsoft.Workflow.VSDesigner.dll',
	r'Common7\IDE\msvb7.dll',
	r'Common7\IDE\WDE.dll',
	r'Common7\IDE\WindowsFormsIntegration.Design.dll',
	r'Common7\IDE\WindowsFormsIntegration.Package.dll',
	r'Common7\IDE\WinFxBrowserApplicationTemplateWizard.dll',
	r'Common7\IDE\WinFxCustomControlTemplateWizard.dll',
	r'Common7\IDE\WorkflowProject.dll',
	r'Common7\IDE\iisresolver.dll',
	r'Common7\IDE\Microsoft.Data.ConnectionUI.Dialog.dll',
	r'Common7\IDE\Microsoft.Data.ConnectionUI.dll',
	r'Common7\IDE\Microsoft.Data.ConnectionUI.xml',
	r'Common7\IDE\Microsoft.VisualStudio.Data.Compatibility.dll',
	r'Common7\IDE\Microsoft.VisualStudio.Data.dll',
	r'Common7\IDE\Microsoft.VisualStudio.Data.Host.dll',
	r'Common7\IDE\Microsoft.VisualStudio.Data.Interop.dll',
	r'Common7\IDE\Microsoft.VisualStudio.Data.Package.dll',
	r'Common7\IDE\Microsoft.VisualStudio.Data.Providers.Common.dll',
	r'Common7\IDE\Microsoft.VisualStudio.Data.Providers.Oracle.dll',
	r'Common7\IDE\Microsoft.VisualStudio.Data.Providers.SqlServer.dll',
	r'Common7\IDE\Microsoft.VisualStudio.DataDesign.SyncDesigner.Dsl.dll',
	r'Common7\IDE\Microsoft.VisualStudio.DataDesign.SyncDesigner.DslPackage.dll',
	r'Common7\IDE\Microsoft.VisualStudio.DataTools.dll',
	r'Common7\IDE\Microsoft.VisualStudio.DataTools.Interop.dll',
	r'Common7\IDE\Microsoft.VisualStudio.DeployWizard.Dll',
	r'Common7\IDE\Microsoft.VisualStudio.vspBatchParser.dll',
	r'Common7\IDE\Microsoft.VisualStudio.vspConnectionInfo.dll',
	r'Common7\IDE\Microsoft.VisualStudio.vspGridControl.dll',
	r'Common7\IDE\Microsoft.VisualStudio.vspRegSvrEnum.dll',
	r'Common7\IDE\Microsoft.VisualStudio.vspServiceBrokerEnum.dll',
	r'Common7\IDE\Microsoft.VisualStudio.vspSmo.dll',
	r'Common7\IDE\Microsoft.VisualStudio.vspSmoEnum.dll',
	r'Common7\IDE\Microsoft.VisualStudio.vspSqlEnum.dll',
	r'Common7\IDE\Microsoft.VisualStudio.vspSqlTDiagM.dll',

	r'Common7\Packages\webdirprj.dll',
	r'Common7\Packages\csspkg.dll',
	r'Common7\Packages\htmdlgs.dll',
	r'Common7\Packages\htmled.dll',
	r'Common7\Packages\srcedit.dll',
	r'Common7\Packages\Debugger\sqlde.dll',
	r'Common7\Packages\Debugger\sqlle.dll',
	r'Common7\Packages\Debugger\cpde.dll',
	r'Common7\Packages\Debugger\cscompee.dll',
	r'Common7\Packages\Debugger\jsee.dll',
	
	r'Common7\IDE\vb7to8.exe',
	r'Common7\IDE\vb7to8DL.dll',
	r'Common7\IDE\vslog.dll',
	r'Common7\IDE\Microsoft.SqlServerCe.Client.dll',
	r'Common7\IDE\Microsoft.VisualStudio.VSPEnumerator.dll',
	r'Common7\IDE\Microsoft.VisualStudio.SmartDevice.Wizard.TestProjectWizards.dll',
	r'Common7\IDE\1033\Microsoft.VisualStudio.Web.ApplicationUI.dll',
	r'Common7\IDE\1033\Microsoft.VisualStudio.WebUI.dll',
	r'Common7\IDE\1033\vb7to8ui.dll'
	]

	#创建备份目录
	dirs = XswUtility.GetPaths(lite_files)
	XswUtility.CreatePaths(dirs, cur_path)

	#移动文件
	for lite_file in lite_files :
		src_file = vs2008_path + '\\' + lite_file
		dst_file = cur_path + '\\' + lite_file
		cmd = r'move "%s" "%s"' % ( src_file, dst_file)
		XswUtility.OutputCmd(cmd)

