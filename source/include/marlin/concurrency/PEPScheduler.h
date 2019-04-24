#ifndef MARLIN_CONCURRENCY_PEPSCHEDULER_h
#define MARLIN_CONCURRENCY_PEPSCHEDULER_h 1

// -- marlin headers
#include <marlin/IScheduler.h>
#include <marlin/Logging.h>
#include <marlin/RandomSeedManager.h>
#include <marlin/concurrency/ThreadPool.h>

// -- std headers
#include <unordered_set>

namespace marlin {

  class ProcessorSequence ;

  namespace concurrency {

    /**
     *  @brief  WorkerOutput struct
     *  Stores the output of a processor sequence call
     */
    struct WorkerOutput {
      ///< The input event
      std::shared_ptr<EVENT::LCEvent>     _event {nullptr} ;
      ///< An exception potential throw in the worker thread
      std::exception_ptr                  _exception {nullptr} ;
    };
    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    /**
     *  @brief  PEPScheduler class
     *  Parallel Event Processing Scheduler.
     *  Implements the scheduling of parallel inter-event processing.
     *
     *  A set of N worker threads are allocated at startup within a thread pool.
     *  Every time a new event is pushed in the scheduler, the event is queued
     *  in the thread pool for further processing. Note that this operation can
     *  fail if the thread pool queue is full. Use freeSlots() to know how many
     *  slots are free in the thread pool queue and avoid unexpected exceptions.
     */
    class PEPScheduler : public IScheduler {
    public:
      using ConditionsMap = std::map<std::string, std::string> ;
      using InputType = std::shared_ptr<EVENT::LCEvent> ;
      using OutputType = WorkerOutput ;
      using WorkerPool = ThreadPool<InputType,OutputType> ;
      using Logger = Logging::Logger ;
      using ProcessorSequences = std::vector<std::shared_ptr<ProcessorSequence>> ;
      using ProcessorSet = std::unordered_set<std::shared_ptr<Processor>> ;
      using PushResultList = std::vector<WorkerPool::PushResult> ;
      using EventList = std::vector<std::shared_ptr<EVENT::LCEvent>> ;

    public:
      PEPScheduler() = default ;

      // from IScheduler interface
      void init( const Application *app ) override ;
      void end() override ;
      void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;
      void pushEvent( std::shared_ptr<EVENT::LCEvent> event ) override ;
      void popFinishedEvents( std::vector<std::shared_ptr<EVENT::LCEvent>> &events ) override ;
      std::size_t freeSlots() const override ;

    private:
      void preConfigure( const Application *app ) ;
      void configureProcessors( const Application *app ) ;
      void configurePool() ;

    private:
      ///< The worker thread pool
      WorkerPool                       _pool {} ;
      ///< The logger instance
      Logger                           _logger {nullptr} ;
      ///< The processor sequences
      ProcessorSequences               _sequences {} ;
      ///< The full list of processor instances
      ProcessorSet                     _allProcessors {} ;
      ///< Initial processor runtime conditions from steering file
      ConditionsMap                    _conditions {} ;
      ///< The random seed manager
      RandomSeedManager                _rdmSeedMgr {} ;
      ///< The list of worker output promises
      PushResultList                   _pushResults {} ;
    };

  }

} // end namespace marlin

#endif