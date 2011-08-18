//---------------------------------------------------------------------------
#ifndef FileOperationProgressH
#define FileOperationProgressH
//---------------------------------------------------------------------------
#include "Configuration.h"
#include "CopyParam.h"
#include "Exceptions.h"
#include <vector>
//---------------------------------------------------------------------------
class TFileOperationProgressType;
enum TFileOperation { foNone, foCopy, foMove, foDelete, foSetProperties,
  foRename, foCustomCommand, foCalculateSize, foRemoteMove, foRemoteCopy,
  foGetProperties, foCalculateChecksum };
enum TCancelStatus { csContinue = 0, csCancel, csCancelTransfer, csRemoteAbort };
enum TResumeStatus { rsNotAvailable, rsEnabled, rsDisabled };
enum TBatchOverwrite { boNo, boAll, boNone, boOlder, boAlternateResume, boAppend, boResume };
typedef void (TObject::*TFileOperationProgressEvent)
  (TFileOperationProgressType & ProgressData, TCancelStatus & Cancel);
typedef void (TObject::*TFileOperationFinished)
  (TFileOperation Operation, TOperationSide Side, bool Temp,
    const wstring & FileName, bool Success, TOnceDoneOperation & OnceDoneOperation);
//---------------------------------------------------------------------------
class TFileOperationProgressType
{
private:
  // when it was last time suspended (to calculate suspend time in Resume())
  unsigned int FSuspendTime;
  // when current file was started being transfered
  TDateTime FFileStartTime;
  int FFilesFinished;
  TFileOperationProgressEvent FOnProgress;
  TFileOperationFinished FOnFinished;
  bool FReset;
  unsigned int FLastSecond;
  unsigned long FRemainingCPS;
  std::vector<unsigned long> FTicks;
  std::vector<__int64> FTotalTransferredThen;

protected:
  void ClearTransfer();
  inline void DoProgress();

public:
  // common data
  TFileOperation Operation;
  // on what side if operation being processed (local/remote), source of copy
  TOperationSide Side;
  wstring FileName;
  wstring Directory;
  bool AsciiTransfer;
  bool TransferingFile;
  bool Temp;

  // file size to read/write
  __int64 LocalSize;
  __int64 LocalyUsed;
  __int64 TransferSize;
  __int64 TransferedSize;
  __int64 SkippedSize;
  TResumeStatus ResumeStatus;
  bool InProgress;
  bool FileInProgress;
  TCancelStatus Cancel;
  int Count;
  // when operation started
  TDateTime StartTime;
  // bytes transfered
  __int64 TotalTransfered;
  __int64 TotalSkipped;
  __int64 TotalSize;

  TBatchOverwrite BatchOverwrite;
  bool SkipToAll;
  unsigned long CPSLimit;

  bool TotalSizeSet;

  bool Suspended;

  TFileOperationProgressType();
  TFileOperationProgressType(
    TFileOperationProgressEvent AOnProgress, TFileOperationFinished AOnFinished);
  ~TFileOperationProgressType();
  void AddLocalyUsed(__int64 ASize);
  void AddTransfered(__int64 ASize, bool AddToTotals = true);
  void AddResumed(__int64 ASize);
  void Clear();
  unsigned int CPS();
  void Finish(wstring FileName, bool Success,
    TOnceDoneOperation & OnceDoneOperation);
  unsigned long LocalBlockSize();
  bool IsLocalyDone();
  bool IsTransferDone();
  void SetFile(wstring AFileName, bool AFileInProgress = true);
  void SetFileInProgress();
  int OperationProgress();
  unsigned long TransferBlockSize();
  unsigned long AdjustToCPSLimit(unsigned long Size);
  static unsigned long StaticBlockSize();
  void Reset();
  void Resume();
  void SetLocalSize(__int64 ASize);
  void SetAsciiTransfer(bool AAsciiTransfer);
  void SetResumeStatus(TResumeStatus AResumeStatus);
  void SetTransferSize(__int64 ASize);
  void ChangeTransferSize(__int64 ASize);
  void RollbackTransfer();
  void SetTotalSize(__int64 ASize);
  void Start(TFileOperation AOperation, TOperationSide ASide, int ACount);
  void Start(TFileOperation AOperation,
    TOperationSide ASide, int ACount, bool ATemp, const wstring ADirectory,
    unsigned long ACPSLimit);
  void Stop();
  void Suspend();
  // whole operation
  TDateTime TimeElapsed();
  // only current file
  TDateTime TimeExpected();
  TDateTime TotalTimeExpected();
  TDateTime TotalTimeLeft();
  int TransferProgress();
  int OverallProgress();
  int TotalTransferProgress();
};
//---------------------------------------------------------------------------
class TSuspendFileOperationProgress
{
public:
  TSuspendFileOperationProgress(TFileOperationProgressType * OperationProgress)
  {
    FOperationProgress = OperationProgress;
    FOperationProgress->Suspend();
  }

  ~TSuspendFileOperationProgress()
  {
    FOperationProgress->Resume();
  }

private:
  TFileOperationProgressType * FOperationProgress;
};
//---------------------------------------------------------------------------
#endif
