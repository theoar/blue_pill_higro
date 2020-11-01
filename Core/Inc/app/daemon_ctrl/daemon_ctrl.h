#ifndef DAEMON_CTRL_H_
#define DAEMON_CTRL_H_

#include "daemon_interface.h"
#include <list>

namespace daemon_ctrl
{

  using namespace std;

  enum class ProcessState
  {
    New, Ready, Running, Waiting, Terminated,
  };

  class Process
  {
    private:
      ProcessState state = ProcessState::New;
      IDaemon *daemon;

      Process()
      {
      }
      ;

    public:

      Process(IDaemon *daemon);

      bool operator ==(const IDaemon *daemon)
      {
	if(this->daemon == daemon)
	  return true;
	else
	  return false;
      }

      bool operator ==(const Process &pr) const
      {
	return (&pr == this) ? true : false;
      }

      void init(void);
      void handler(void);
      void terminate(void);
      void start(void);
      void stop(void);
      ProcessState getState(void);
  };

  class Daemon
  {

    private:
      list<Process*> processes;
      Process* findProcess(IDaemon *deamon);
      bool checkExistProcess(Process *process);

    public:
      Daemon();

      void init(void);

      Process* addProcess(IDaemon *deamon);
      Process* addAndStartProcess(IDaemon *deamon);
      bool terminateProcess(Process *process);
      bool startProcess(Process *process);
      bool stopProcess(Process *process);

      void handler(void);

      ~Daemon();
  };

}

#endif

