#include "app/daemon_ctrl/daemon_ctrl.h"
#include "app/daemon_ctrl/daemon_interface.h"

#include <list>

using namespace daemon_ctrl;
using namespace std;

Process::Process(IDaemon *daemon)
{
  state = ProcessState::New;
  this->daemon = daemon;
}

void Process::init(void)
{
  if(state == ProcessState::New)
  {
    state = ProcessState::Ready;
    daemon->init();
  }
}

void Process::handler(void)
{
  if(state == ProcessState::Running)
  {
    daemon->handler();
  }
}

void Process::terminate(void)
{
  if(state != ProcessState::Terminated)
  {
    state = ProcessState::Terminated;
    daemon->terminate();
  }
}

void Process::start(void)
{
  if(state == ProcessState::Ready || state == ProcessState::Waiting)
  {
    state = ProcessState::Running;
    daemon->start();
  }
}

void Process::stop(void)
{
  if(state == ProcessState::Running)
  {
    state = ProcessState::Waiting;
    daemon->stop();
  }
}

ProcessState Process::getState(void)
{
  return this->state;
}

Daemon::Daemon()
{

}

Process* Daemon::findProcess(IDaemon *daemon)
{
  for(list<Process*>::iterator process = processes.begin(); process != processes.end(); ++process)
  {
    if(*(*process) == daemon)
    {
      return *process;
    }
  }
  return nullptr;
}

Process* Daemon::addProcess(IDaemon *daemon)
{
  Process *process = new Process(daemon);
  processes.push_back(process);
  process->init();
  return process;
}

Process* Daemon::addAndStartProcess(IDaemon *daemon)
{
  Process *process = addProcess(daemon);
  process->start();
  return process;
}

bool Daemon::terminateProcess(Process *process)
{
  if(checkExistProcess(process))
  {
    process->terminate();
    processes.remove(process);
    return true;
  }
  return false;
}

bool Daemon::checkExistProcess(Process *process)
{
  for(list<Process*>::iterator it = processes.begin(); it != processes.end(); ++it)
  {
    if(*it == process)
      return true;
  }
  return false;
}

bool Daemon::startProcess(Process *process)
{
  if(checkExistProcess(process))
  {
    process->start();
    return true;
  }
  return false;
}

bool Daemon::stopProcess(Process *process)
{
  if(checkExistProcess(process))
  {
    process->stop();
    return true;
  }
  return false;
}

void Daemon::handler(void)
{
  for(auto x : this->processes)
    x->handler();
}

Daemon::~Daemon()
{

}

