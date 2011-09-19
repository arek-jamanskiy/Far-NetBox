//---------------------------------------------------------------------------
#ifndef WinSCPFileSystemH
#define WinSCPFileSystemH
//---------------------------------------------------------------------------
#include "boostdefines.hpp"
#include <boost/signals/signal3.hpp>

#include "Interface.h"
#include "FarPlugin.h"
#include "FileOperationProgress.h"
#include <Terminal.h>
#include <GUIConfiguration.h>
#include <SynchronizeController.h>
#include <Queue.h>
#include <list>
#include <map>
//---------------------------------------------------------------------------
class TTerminal;
class TSessionData;
class TRemoteFile;
class TBookmarkList;
class TWinSCPPlugin;
class TFarButton;
class TFarDialogItem;
class TFarDialog;
class TTerminalQueue;
class TTerminalQueueStatus;
class TQueueItem;
class TKeepaliveThread;
struct TMessageParams;
#define REMOTE_DIR_HISTORY L"WinscpRemoteDirectory"
#define ASCII_MASK_HISTORY L"WinscpAsciiMask"
#define LINK_FILENAME_HISTORY L"WinscpRemoteLink"
#define LINK_POINT_TO_HISTORY L"WinscpRemoteLinkPointTo"
#define APPLY_COMMAND_HISTORY L"WinscpApplyCmd"
#define APPLY_COMMAND_PARAM_HISTORY L"WinscpApplyCmdParam"
#define LOG_FILE_HISTORY L"WinscpLogFile"
#define REMOTE_SYNC_HISTORY L"WinscpRemoteSync"
#define LOCAL_SYNC_HISTORY L"WinscpLocalSync"
#define MOVE_TO_HISTORY L"WinscpMoveTo"
#define EXCLUDE_FILE_MASK_HISTORY L"WinscpExcludeFileMask"
#define MAKE_SESSION_FOLDER_HISTORY L"WinscpSessionFolder"
//---------------------------------------------------------------------------
// for Properties dialog
const int cpMode =  0x01;
const int cpOwner = 0x02;
const int cpGroup = 0x04;
// for Copy dialog
const int coTempTransfer        = 0x01;
const int coDisableNewerOnly    = 0x04;
// for Synchronize and FullSynchronize dialogs
const int spSelectedOnly = 0x800;
// for Synchronize dialogs
const int soAllowSelectedOnly = 0x01;
// for FullSynchronize dialog
const int fsoDisableTimestamp = 0x01;
const int fsoAllowSelectedOnly = 0x02;
enum TSessionActionEnum { saAdd, saEdit, saConnect };
//---------------------------------------------------------------------------
// typedef void (TObject::*TGetSynchronizeOptionsEvent)
  // (int Params, TSynchronizeOptions & Options);
typedef boost::signal2<void, int, TSynchronizeOptions &> getsynchronizeoptions_signal_type;
typedef getsynchronizeoptions_signal_type::slot_type getsynchronizeoptions_slot_type;
// typedef void (TObject::*TGetSpaceAvailable)
  // (const std::wstring Path, TSpaceAvailable & ASpaceAvailable, bool & Close);
typedef boost::signal3<void, const std::wstring, TSpaceAvailable &, bool &> getspaceavailable_signal_type;
typedef getspaceavailable_signal_type::slot_type getspaceavailable_slot_type;
struct TMultipleEdit
{
  std::wstring FileName;
  std::wstring Directory;
  std::wstring LocalFileName;
  bool PendingSave;
};
struct TEditHistory
{
  std::wstring FileName;
  std::wstring Directory;
  bool operator==(const TEditHistory& rh) { return (FileName == rh.FileName) && (Directory == rh.Directory); }
};
//---------------------------------------------------------------------------
// typedef void (TObject::* TProcessSessionEvent)(TSessionData * Data, void * Param);
typedef boost::signal2<void, TSessionData *, void *> processsession_signal_type;
typedef processsession_signal_type::slot_type processsession_slot_type;
//---------------------------------------------------------------------------
class TWinSCPFileSystem : public TCustomFarFileSystem
{
friend class TWinSCPPlugin;
friend class TKeepaliveThread;
friend class TQueueDialog;
public:
  TWinSCPFileSystem(TCustomFarPlugin * APlugin);
  virtual ~TWinSCPFileSystem();

  virtual void Close();

protected:
  bool Connect(TSessionData * Data);
  void SaveSession();

  virtual void GetOpenPluginInfoEx(long unsigned & Flags,
    std::wstring & HostFile, std::wstring & CurDir, std::wstring & Format,
    std::wstring & PanelTitle, TFarPanelModes * PanelModes, int & StartPanelMode,
    int & StartSortMode, bool & StartSortOrder, TFarKeyBarTitles * KeyBarTitles,
    std::wstring & ShortcutData);
  virtual bool GetFindDataEx(TObjectList *PanelItems, int OpMode);
  virtual bool ProcessKeyEx(int Key, unsigned int ControlState);
  virtual bool SetDirectoryEx(const std::wstring Dir, int OpMode);
  virtual int MakeDirectoryEx(std::wstring & Name, int OpMode);
  virtual bool DeleteFilesEx(TList * PanelItems, int OpMode);
  virtual int GetFilesEx(TList * PanelItems, bool Move,
    std::wstring & DestPath, int OpMode);
  virtual int PutFilesEx(TList * PanelItems, bool Move, int OpMode);
  virtual bool ProcessEventEx(int Event, void * Param);

  void ProcessEditorEvent(int Event, void * Param);

  virtual void HandleException(const std::exception * E, int OpMode = 0);
  void KeepaliveThreadCallback();

  inline bool SessionList();
  inline bool Connected();
  TWinSCPPlugin * WinSCPPlugin();
  void ShowOperationProgress(TFileOperationProgressType & ProgressData,
    bool Force);
  bool SessionDialog(TSessionData * Data, TSessionActionEnum & Action);
  void EditConnectSession(TSessionData * Data, bool Edit);
  void DuplicateRenameSession(TSessionData * Data,
    bool Duplicate);
  void FocusSession(TSessionData * Data);
  void DeleteSession(TSessionData * Data, void * Param);
  void ProcessSessions(TList * PanelItems,
    const processsession_slot_type &ProcessSession, void * Param);
  void ExportSession(TSessionData * Data, void * Param);
  bool ImportSessions(TList * PanelItems, bool Move, int OpMode);
  void FileProperties();
  void CreateLink();
  void TransferFiles(bool Move);
  void RenameFile();
  void ApplyCommand();
  void ShowInformation();
  void InsertTokenOnCommandLine(std::wstring Token, bool Separate);
  void InsertSessionNameOnCommandLine();
  void InsertFileNameOnCommandLine(bool Full);
  void InsertPathOnCommandLine();
  void CopyFullFileNamesToClipboard();
  void FullSynchronize(bool Source);
  void Synchronize();
  void OpenDirectory(bool Add);
  void HomeDirectory();
  void ToggleSynchronizeBrowsing();
  bool IsSynchronizedBrowsing();
  bool PropertiesDialog(TStrings * FileList,
    const std::wstring Directory, 
    // const TRemoteTokenList *GroupList, const TRemoteTokenList *UserList,
    TStrings * GroupList, TStrings * UserList,
    TRemoteProperties * Properties, int AllowedChanges);
  bool ExecuteCommand(const std::wstring Command);
  void TerminalCaptureLog(const std::wstring & AddedLine, bool StdError);
  bool CopyDialog(bool ToRemote, bool Move, TStrings * FileList,
    std::wstring & TargetDirectory, 
    // TGUICopyParamType * Params,
    TCopyParamType * Params,
    int Options,
    int CopyParamAttrs);
  bool LinkDialog(std::wstring & FileName, std::wstring & PointTo, bool & Symbolic,
    bool Edit, bool AllowSymbolic);
  void FileSystemInfoDialog(const TSessionInfo & SessionInfo,
    const TFileSystemInfo & FileSystemInfo, std::wstring SpaceAvailablePath,
    const getspaceavailable_slot_type &OnGetSpaceAvailable);
  bool OpenDirectoryDialog(bool Add, std::wstring & Directory,
    TBookmarkList * BookmarkList);
  bool ApplyCommandDialog(std::wstring & Command, int & Params);
  bool FullSynchronizeDialog(TTerminal::TSynchronizeMode & Mode,
    int & Params, std::wstring & LocalDirectory, std::wstring & RemoteDirectory,
    TCopyParamType * CopyParams, bool & SaveSettings, bool & SaveMode, int Options,
    const TUsableCopyParamAttrs & CopyParamAttrs);
  bool SynchronizeChecklistDialog(TSynchronizeChecklist * Checklist,
    TTerminal::TSynchronizeMode Mode, int Params,
    const std::wstring LocalDirectory, const std::wstring RemoteDirectory);
  bool RemoteTransferDialog(TStrings * FileList, std::wstring & Target,
    std::wstring & FileMask, bool Move);
  bool RenameFileDialog(TRemoteFile * File, std::wstring & NewName);
  int MoreMessageDialog(std::wstring Str, TStrings * MoreMessages,
    TQueryType Type, int Answers, const TMessageParams * Params = NULL);
  bool PasswordDialog(TSessionData * SessionData,
    TPromptKind Kind, std::wstring Name, std::wstring Instructions, TStrings * Prompts,
    TStrings * Results, bool StoredCredentialsTried);
  bool BannerDialog(std::wstring SessionName, const std::wstring & Banner,
    bool & NeverShowAgain, int Options);
  bool CreateDirectoryDialog(std::wstring & Directory,
    TRemoteProperties * Properties, bool & SaveSettings);
  bool QueueDialog(TTerminalQueueStatus * Status, bool ClosingPlugin);
  bool SynchronizeDialog(TSynchronizeParamType & Params,
    const TCopyParamType * CopyParams, TSynchronizeStartStopEvent OnStartStop,
    bool & SaveSettings, int Options, int CopyParamAttrs,
    const getsynchronizeoptions_slot_type &OnGetOptions);
  void DoSynchronize(TSynchronizeController * Sender,
    const std::wstring LocalDirectory, const std::wstring RemoteDirectory,
    const TCopyParamType & CopyParam, const TSynchronizeParamType & Params,
    TSynchronizeChecklist ** Checklist, TSynchronizeOptions * Options, bool Full);
  void DoSynchronizeInvalid(TSynchronizeController * Sender,
    const std::wstring Directory, const std::wstring ErrorStr);
  void DoSynchronizeTooManyDirectories(TSynchronizeController * Sender,
    int & MaxDirectories);
  void Synchronize(const std::wstring LocalDirectory,
    const std::wstring RemoteDirectory, TTerminal::TSynchronizeMode Mode,
    const TCopyParamType & CopyParam, int Params, TSynchronizeChecklist ** Checklist,
    TSynchronizeOptions * Options);
  bool SynchronizeAllowSelectedOnly();
  void GetSynchronizeOptions(int Params, TSynchronizeOptions & Options);
  void RequireCapability(int Capability);
  void RequireLocalPanel(TFarPanelInfo * Panel, std::wstring Message);
  bool AreCachesEmpty();
  void ClearCaches();
  void OpenSessionInPutty();
  void QueueShow(bool ClosingPlugin);
  TTerminalQueueStatus * ProcessQueue(bool Hidden);
  bool EnsureCommandSessionFallback(TFSCapability Capability);
  void ConnectTerminal(TTerminal * Terminal);
  void TemporarilyDownloadFiles(TStrings * FileList,
    TCopyParamType CopyParam, std::wstring & TempDir);
  int UploadFiles(bool Move, int OpMode, bool Edit, std::wstring DestPath);
  void UploadOnSave(bool NoReload);
  void UploadFromEditor(bool NoReload, std::wstring FileName, std::wstring DestPath);
  void LogAuthentication(TTerminal * Terminal, std::wstring Msg);
  void MultipleEdit();
  void MultipleEdit(std::wstring Directory, std::wstring FileName, TRemoteFile * File);
  void EditViewCopyParam(TCopyParamType & CopyParam);
  bool SynchronizeBrowsing(std::wstring NewPath);
  bool IsEditHistoryEmpty();
  void EditHistory();
  std::wstring ProgressBar(int Percentage, int Width);
  bool IsLogging();
  void ShowLog();

  // __property  TTerminal * Terminal = { read = FTerminal };
  TTerminal * GetTerminal() { return FTerminal; }

private:
  TTerminal * FTerminal;
  TTerminalQueue * FQueue;
  TTerminalQueueStatus * FQueueStatus;
  TCriticalSection * FQueueStatusSection;
  bool FQueueStatusInvalidated;
  bool FQueueItemInvalidated;
  bool FRefreshLocalDirectory;
  bool FRefreshRemoteDirectory;
  bool FQueueEventPending;
  TQueueEvent FQueueEvent;
  bool FReloadDirectory;
  HANDLE FProgressSaveScreenHandle;
  HANDLE FSynchronizationSaveScreenHandle;
  HANDLE FAuthenticationSaveScreenHandle;
  TDateTime FSynchronizationStart;
  bool FSynchronizationCompare;
  TStrings * FFileList;
  TList * FPanelItems;
  std::wstring FSavedFindFolder;
  std::wstring FOriginalEditFile;
  std::wstring FLastEditFile;
  std::wstring FLastMultipleEditFile;
  std::wstring FLastMultipleEditDirectory;
  bool FLastMultipleEditReadOnly;
  int FLastEditorID;
  bool FEditorPendingSave;
  TCopyParamType FLastEditCopyParam;
  bool FNoProgress;
  bool FNoProgressFinish;
  TKeepaliveThread * FKeepaliveThread;
  bool FSynchronisingBrowse;
  TSynchronizeController * FSynchronizeController;
  TStrings * FCapturedLog;
  bool FOutputLog;
  TStrings * FAuthenticationLog;
  TWinSCPFileSystem *Self;
  typedef std::map<int, TMultipleEdit> TMultipleEdits;
  TMultipleEdits FMultipleEdits;
  bool FLoadingSessionList;
  typedef std::vector<TEditHistory> TEditHistories;
  TEditHistories FEditHistories;
  std::wstring FLastPath;
  TStrings * FPathHistory;
  std::wstring FSessionsFolder;
  std::wstring FNewSessionsFolder;

  void TerminalClose(TObject * Sender);
  void TerminalUpdateStatus(TTerminal * Terminal, bool Active);
  void TerminalChangeDirectory(TObject * Sender);
  void TerminalReadDirectory(TObject* Sender, bool ReloadOnly);
  void TerminalStartReadDirectory(TObject * Sender);
  void TerminalReadDirectoryProgress(TObject * Sender, int Progress,
    bool & Cancel);
  void TerminalInformation(TTerminal * Terminal,
    const std::wstring & Str, bool Status, bool Active);
  void TerminalQueryUser(TObject * Sender,
    const std::wstring Query, TStrings * MoreMessages, int Answers,
    const TQueryParams * Params, int & Answer, TQueryType Type, void * Arg);
  void TerminalPromptUser(TTerminal * Terminal,
    TPromptKind Kind, std::wstring Name, std::wstring Instructions,
    TStrings * Prompts, TStrings * Results, bool & Result,
    void * Arg);
  void TerminalDisplayBanner(TTerminal * Terminal,
    std::wstring SessionName, const std::wstring & Banner, bool & NeverShowAgain,
    int Options);
  void TerminalShowExtendedException(TTerminal * Terminal,
    const std::exception * E, void * Arg);
  void TerminalDeleteLocalFile(const std::wstring FileName, bool Alternative);
  void OperationProgress(
    TFileOperationProgressType & ProgressData, TCancelStatus & Cancel);
  void OperationFinished(TFileOperation Operation,
    TOperationSide Side, bool DragDrop, const std::wstring & FileName, bool Success,
    bool & DisconnectWhenComplete);
  void CancelConfiguration(TFileOperationProgressType & ProgressData);
  TStrings * CreateFileList(TList * PanelItems,
    TOperationSide Side, bool SelectedOnly = false, std::wstring Directory = L"",
    bool FileNameOnly = false, TStrings * AFileList = NULL);
  TStrings * CreateSelectedFileList(TOperationSide Side,
    TFarPanelInfo * PanelInfo = NULL);
  TStrings * CreateFocusedFileList(TOperationSide Side,
    TFarPanelInfo * PanelInfo = NULL);
  void CustomCommandGetParamValue(
    const std::wstring AName, std::wstring & Value);
  void TerminalSynchronizeDirectory(const std::wstring LocalDirectory,
    const std::wstring RemoteDirectory, bool & Continue, bool Collect);
  void QueueListUpdate(TTerminalQueue * Queue);
  void QueueItemUpdate(TTerminalQueue * Queue, TQueueItem * Item);
  void QueueEvent(TTerminalQueue * Queue, TQueueEvent Event);
  void GetSpaceAvailable(const std::wstring Path,
    TSpaceAvailable & ASpaceAvailable, bool & Close);
  void QueueAddItem(TQueueItem * Item);
};
//---------------------------------------------------------------------------
class TSessionPanelItem : public TCustomFarPanelItem
{
public:
  TSessionPanelItem(std::wstring Path);
  TSessionPanelItem(TSessionData * ASessionData);
  static void SetPanelModes(TFarPanelModes * PanelModes);
  static void SetKeyBarTitles(TFarKeyBarTitles * KeyBarTitles);

protected:
  std::wstring FPath;
  TSessionData * FSessionData;

  virtual void GetData(
    unsigned long & Flags, std::wstring & FileName, __int64 & Size,
    unsigned long & FileAttributes,
    TDateTime & LastWriteTime, TDateTime & LastAccess,
    unsigned long & NumberOfLinks, std::wstring & Description,
    std::wstring & Owner, void *& UserData, int & CustomColumnNumber);
};
//---------------------------------------------------------------------------
class TSessionFolderPanelItem : public TCustomFarPanelItem
{
public:
  TSessionFolderPanelItem(std::wstring Folder);

protected:
  std::wstring FFolder;

  virtual void GetData(
    unsigned long & Flags, std::wstring & FileName, __int64 & Size,
    unsigned long & FileAttributes,
    TDateTime & LastWriteTime, TDateTime & LastAccess,
    unsigned long & NumberOfLinks, std::wstring & Description,
    std::wstring & Owner, void *& UserData, int & CustomColumnNumber);
};
//---------------------------------------------------------------------------
class TRemoteFilePanelItem : public TCustomFarPanelItem
{
public:
  TRemoteFilePanelItem(TRemoteFile * ARemoteFile);
  static void SetPanelModes(TFarPanelModes * PanelModes);
  static void SetKeyBarTitles(TFarKeyBarTitles * KeyBarTitles);

protected:
  TRemoteFile * FRemoteFile;

  virtual void GetData(
    unsigned long & Flags, std::wstring & FileName, __int64 & Size,
    unsigned long & FileAttributes,
    TDateTime & LastWriteTime, TDateTime & LastAccess,
    unsigned long & NumberOfLinks, std::wstring & Description,
    std::wstring & Owner, void *& UserData, int & CustomColumnNumber);
  virtual std::wstring CustomColumnData(int Column);
  static void TranslateColumnTypes(std::wstring & ColumnTypes,
    TStrings * ColumnTitles);
};
//---------------------------------------------------------------------------
#endif
