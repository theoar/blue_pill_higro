#ifndef DAEMON_INTERFACE_H_
#define DAEMON_INTERFACE_H_

namespace daemon_ctrl
{

  class IDaemon
  {
    public:

      virtual ~IDaemon()
      {
      }
      virtual void init(void)
      {
      }
      ;
      virtual void handler(void) = 0;
      virtual void terminate(void)
      {
      }
      ;
      virtual void start(void)
      {
      }
      ;
      virtual void stop(void)
      {
      }
      ;
  };

}

#endif

