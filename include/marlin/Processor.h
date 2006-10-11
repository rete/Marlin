#ifndef Processor_h
#define Processor_h 1

#include "lcio.h"

#include "IO/LCRunListener.h"
#include "IO/LCEventListener.h"
#include "IO/LCReader.h"

#include "EVENT/LCEvent.h"
#include "EVENT/LCRunHeader.h"

#include "StringParameters.h"
#include "ProcessorParameter.h"
#include <map>

using namespace lcio ;


namespace marlin{

  class ProcessorMgr ;
  //  class ProcessorParameter ;

  typedef std::map<std::string, ProcessorParameter* > ProcParamMap ;
  typedef std::map<std::string, std::string >         LCIOTypeMap ;

  /** Base class for Marlin processors.
   * Users can optionaly overwrite the following methods: <br>
   *   init, processRun, processEvent and end.<br>
   * Use registerProcessorParameter to define all parameters that the module uses.
   * Registered parameters are filled automatically before init() is called.
   * With MyAplication -l you can print a list of available processors including
   * the steering parameters they use/need.<br>
   * With MyAplication -x you can print an example XML steering file for all known
   * processors.
   *
   * @see init 
   * @see processRun
   * @see processEvent
   * @see end
   *
   *  @author F. Gaede, DESY
   *  @version $Id: Processor.h,v 1.16 2006-10-11 09:32:17 gaede Exp $ 
   */
  
  class Processor {
  
    friend class ProcessorMgr ;

  public:
  
    /** Default constructor - subclasses need to call this in their
     * default constructor.
     */
    Processor(const std::string& typeName) ; 

    /** Destructor */
    virtual ~Processor() ; 
  
  
    /**Return a new instance of the processor.
     * Has to be implemented by subclasses.
     */
    virtual Processor*  newProcessor() = 0 ;
  

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    virtual void init() { }

    /** Called for every run, e.g. overwrite to initialize run dependent 
     *  histograms.
     */
    virtual void processRunHeader( LCRunHeader* run) { } 

    /** Called for every event - the working horse. 
     */
    virtual void processEvent( LCEvent * evt ) { }

    /** Called for every event - right after processEvent()
     *  has been called for all processors.
     *  Use to check processing and/or produce check plots.
     */
    virtual void check( LCEvent * evt ) { }


    /** Called after data processing for clean up in the inverse order of the init()
     *  method so that resources allocated in the first processor also will be available
     *  for all following processors.
     */
    virtual void end(){ }
  

    /** Return type name for the processor (as set in constructor).
     */
    virtual const std::string & type() const { return _typeName ; } 

    /** Return  name of this  processor.
     */
    virtual const std::string & name() const { return _processorName ; } 

    /** Return parameters defined for this Processor.
     */
    virtual StringParameters* parameters() { return _parameters ; } 


    /** Print information about this processor in ASCII steering file format.
     */
    virtual void printDescription() ;

    /** Print information about this processor in XML steering file format.
     */
    virtual void printDescriptionXML() ;

    /** Print the parameters and its values.
     */
    virtual void printParameters() ;

    /** Description of processor.
     */
    const std::string& description() { return _description ; }


    /** True if first event in processEvent(evt) - use this e.g. to initialize histograms etc.
     */
    bool isFirstEvent() { return _isFirstEvent ; } ;

  protected:

    /** Set the return value for this processor - typically at end of processEvent(). 
     *  The value can be used in a condition in the steering file referred to by the name
     *  of the processor. 
     */
    void setReturnValue( bool val) ;

    /** Set a named return value for this processor - typically at end of processEvent() 
     *  The value can be used in a condition in the steering file referred to by 
     *  ProcessorName.name of the processor. 
     */
    void setReturnValue( const std::string& name, bool val ) ;


    /** Register a steering variable for this processor - call in constructor of processor.
     *  The default value has to be of the _same_ type as the parameter, e.g.<br>
     *  float _cut ;<br>
     *  ...<br>
     *   registerProcessorParameter( "Cut", "cut...", _cut , float( 3.141592 ) ) ;<br>
     *  as implicit conversions don't work for templates.<br>
     *  The optional parameter setSize is used for formating the printout of parameters.
     *  This can be used if the parameter values are expected to come in sets of fixed size. 
     */
     template<class T>
     void registerProcessorParameter(const std::string& name, 
 				    const std::string&description,
 				    T& parameter,
 				    const T& defaultVal,
				    int setSize=0 ) {
    
       _map[ name ] = new ProcessorParameter_t<T>( name , description, 
						   parameter, defaultVal, 
						   false , setSize) ;
     }
    
    /** Specialization of registerProcessorParameter() for a parameter that defines an 
     *  input collection - can be used fo checking the consistency of the steering file.
     */
    void registerInputCollection(const std::string& type,
				 const std::string& name, 
				 const std::string&description,
				 std::string& parameter,
				 const std::string& defaultVal,
				 int setSize=0 ) {
      
      setLCIOInType( name , type ) ;
      registerProcessorParameter( name, description, parameter, defaultVal, setSize ) ; 
    }
    
    /** Specialization of registerProcessorParameter() for a parameter that defines an 
     *  output collection - can be used fo checking the consistency of the steering file.
     */
    void registerOutputCollection(const std::string& type,
				  const std::string& name, 
				  const std::string&description,
				  std::string& parameter,
				  const std::string& defaultVal,
				  int setSize=0 ) {
      
      setLCIOOutType( name , type ) ;
      registerProcessorParameter( name, description, parameter, defaultVal, setSize ) ; 
    }

    /** Specialization of registerProcessorParameter() for a parameter that defines one or several 
     *  input collections - can be used fo checking the consistency of the steering file.
     */
    void registerInputCollections(const std::string& type,
				  const std::string& name, 
				  const std::string&description,
				  StringVec& parameter,
				  const StringVec& defaultVal,
				  int setSize=0 ) {
      
      setLCIOInType( name , type ) ;
      registerProcessorParameter( name, description, parameter, defaultVal, setSize ) ; 
    }
    
    /** Same as  registerProcessorParameter except that the parameter is optional.
     *  The value of the parameter will still be set to the default value, which
     *  is used to print an example steering line.
     *  Use parameterSet() to check whether it actually has been set in the steering 
     *  file.
     */
    template<class T>
    void registerOptionalParameter(const std::string& name, 
				   const std::string&description,
				   T& parameter,
				   const T& defaultVal,
				   int setSize=0 ) {
      
      _map[ name ] = new ProcessorParameter_t<T>( name , description, 
						  parameter, defaultVal, 
						  true , setSize) ;
    }
    
    /** Tests whether the parameter has been set in the steering file
     */
    bool parameterSet( const std::string& name ) ;
    
    
  private: // called by ProcessorMgr
    
    /** Set processor name */
    virtual void setName( const std::string & name) { _processorName = name ; }
    
    /** Initialize the parameters */
    virtual void setParameters( StringParameters* parameters) ; 
    
    /** Sets the registered steering parameters before calling init() 
     */
    virtual void baseInit() ;
    
    /** Called by ProcessorMgr */
    void setFirstEvent( bool isFirstEvent ) { _isFirstEvent =  isFirstEvent ; }

    // called internally
    
    /** Set the expected LCIO type for a parameter that refers to one or more 
     *  input collections, e.g.:<br>
     *  &nbsp;&nbsp;  setReturnValue( "InputCollecitonName" , LCIO::MCPARTICLE ) ; <br>
     *  Set to LCIO::LCObject if more than one type is allowed, e.g. for a viewer processor.
     */
    void setLCIOInType(const std::string& colName,  const std::string& lcioInType) ;
    
    /** True if the given parameter defines an LCIO input collection, i.e. the type has 
     *  been defined with setLCIOInType().
     */

    bool isInputCollectionName( const std::string& parameterName  ) ;  


    /** Set the  LCIO type for a parameter that refers to an output collections, i.e. the type has 
     *  been defined with setLCIOOutType().
     */
    void setLCIOOutType(const std::string& collectionName,  const std::string& lcioOutType) ;
    

    /** True if the given parameter defines an LCIO output collection */
    bool isOutputCollectionName( const std::string& parameterName  ) ;  


  protected:

    /**Describes what the processor does. Set in constructor.
     */
    std::string _description ;
    std::string _typeName  ;
    std::string _processorName ;
    StringParameters* _parameters ;

    ProcParamMap _map ;  
    bool _isFirstEvent ;
    LCIOTypeMap   _inTypeMap ;
    LCIOTypeMap   _outTypeMap ;

  private:
    Processor() ; 
  
  };
 
} // end namespace marlin 

#endif
