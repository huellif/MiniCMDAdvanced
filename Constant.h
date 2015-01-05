/*
 ============================================================================
 Name           : MiniCMD
 Author         : Just Fancy
 Project Home   : http://code.google.com/p/minicmd/
 ============================================================================
    MiniCMD, mini commander for Symbian OS phone
    Copyright (C) 2011  Just Fancy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ============================================================================
 */
/*
_LIT(KDefCMDFileD, "D:\\Data\\MiniCmd.txt");
_LIT(KDefCMDFileE, "E:\\Data\\MiniCmd.txt");
_LIT(KDefCMDFileC, "C:\\Data\\MiniCmd.txt");
_LIT(KDefCMDFileZ, "Z:\\Data\\MiniCmd.txt");

_LIT(KDisableFile, "MiniCmd.disable");
*/
_LIT(KLogFmt,       "] [%d]");

_LIT(KCmdAbort,     "abort");
_LIT(KCmdAtt,       "att");
_LIT(KCmdCp,        "cp");
_LIT(KCmdCopy,      "copy");
_LIT(KCmdMv,        "mv");
_LIT(KCmdMd,        "md");
_LIT(KCmdMkDir,     "mkdir");
_LIT(KCmdRm,        "rm");
_LIT(KCmdRmDir,     "rmdir");
_LIT(KCmdRn,        "rn");
_LIT(KCmdRename,    "rename");
_LIT(KCmdDel,       "delete");
_LIT(KCmdRun,       "run");
_LIT(KCmdKill,      "kill");
_LIT(KCmdSleep,     "sleep");
_LIT(KCmdAbes,      "abort_if_exists");
_LIT(KCmdAbnes,     "abort_if_not_exists");
_LIT(KCmdSkes,      "skip_if_exists");
_LIT(KCmdSknes,     "skip_if_not_exists");
_LIT(KCmdBkes,      "back_if_exists");
_LIT(KCmdBknes,     "back_if_not_exists");
_LIT(KCmdRses,      "restart_if_exists");
_LIT(KCmdRsnes,     "restart_if_not_exists");
_LIT(KCmdIf,        "if");
_LIT(KCmdIfn,       "ifn");
_LIT(KCmdElseIf,    "elseif");
_LIT(KCmdElseIfn,   "elseifn");
_LIT(KCmdElse,      "else");
_LIT(KCmdEndIf,     "endif");
_LIT(KCmdCmd,       "cmd");
_LIT(KCmdKey,       "key");
_LIT(KCmdSendKey,   "sendkey");
_LIT(KCmdFile,      "file");
_LIT(KCmdNote,      "note");
_LIT(KCmdLog,       "log");
_LIT(KCmdStop,      "stop");
_LIT(KCmdFind,      "find");
_LIT(KCmdInstall,   "install");
_LIT(KCmdUninstall, "uninstall");
_LIT(KCmdLogPs,		"logps");
